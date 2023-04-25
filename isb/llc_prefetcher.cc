#include "cache.h"
#include "ooo_cpu.h"

#include <unordered_map>

// #define SIG_DEBUG_PRINT
#ifdef SIG_DEBUG_PRINT
#define SIG_DP(x) x
#else
#define SIG_DP(x)
#endif

uint32_t prefetch_degree = 4;

void update_structural_address(uint64_t new_str_addr, uint64_t phy_addr){
    sp_cache_[new_str_addr].phy_addr = phy_addr;
    sp_cache_[new_str_addr].valid = 1;

}

void CACHE::l1d_prefetcher_initialize() 
{
    
}

void CACHE::l1d_prefetcher_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint64_t virtual_addr)
{
    if(training_unit.find(ip) == training_unit.end() || training_unit[ip].valid == 0){
        training_unit[ip].last_addr = addr;
        training_unit[ip].valid = 1;
        return;
    }
    
    if(training_unit.find(ip) != training_unit.end()){  // the instruction pointer exists in the training table
        if(training_unit[ip].valid == 1){  // valid bit is set, implies it is valid
            // A = last_addr, B = addr
            // check if A is available in PSAMAP
            uint64_t last_addr = training_unit[ip].last_addr;

            uint8_t ispres_A = 1;
            if(ps_cache_.find(last_addr) == ps_cache_.end() || ps_cache_[last_addr].valid == 0){
                ispres_A = 0;
            }

            uint8_t ispres_B = 1;
            if(ps_cache_.find(addr) == ps_cache_.end() || ps_cache_[addr].valid == 0){
                ispres_B = 0;
            }

            if((ispres_A == 1) && (ispres_B == 1)){
                if(ps_cache_[addr].str_addr == ps_cache_[last_addr].str_addr + 1){
                    ps_cache_[addr].counter++;
                }
                else{
                    ps_cache_[addr].counter--;
                    if(ps_cache_[addr].counter <= 0){
                        assert(ps_cache_[addr].counter == 0);
                        if(ps_cache_[last_addr].index_in_stream < MAX_STREAM_SIZE - 1){
                            uint64_t new_str_addr = ps_cache_[last_addr].str_addr + 1;
                            ps_cache_[addr].str_addr = new_str_addr;
                            ps_cache_[addr].index_in_stream = ps_cache_[last_addr].index_in_stream + 1;
                            update_structural_address(new_str_addr, addr);
                        }
                        else{
                            assert(ps_cache_[last_addr].index_in_stream == MAX_STREAM_SIZE - 1);
                            ps_cache_[addr].str_addr = str_addr_counter;
                            ps_cache_[addr].valid = 1;
                            ps_cache_[addr].index_in_stream = 0;
                            update_structural_address(str_addr_counter, addr);
                            str_addr_counter += MAX_STREAM_SIZE;
                        }
                        
                        ps_cache_[addr].counter = 1;                        
                     }
                }
            }

            else if(ispres_A == 0){
                
                ps_cache_[last_addr].str_addr = str_addr_counter;
                ps_cache_[last_addr].valid = 1;
                ps_cache_[last_addr].counter = 1;
                ps_cache_[last_addr].index_in_stream = 0;
                ps_cache_[addr].str_addr = str_addr_counter + 1;
                ps_cache_[addr].valid = 1;
                ps_cache_[addr].counter = 1;
                ps_cache_[addr].index_in_stream = 1;

                update_structural_address(str_addr_counter, last_addr);
                update_structural_address(str_addr_counter + 1, addr);

                str_addr_counter += MAX_STREAM_SIZE;   // increase the value of c for next temporal stream
            }


            else if(ispres_B == 0){
                if(ps_cache_[last_addr].index_in_stream < MAX_STREAM_SIZE - 1){
                    uint64_t new_str_addr = ps_cache_[last_addr].str_addr + 1;
                    ps_cache_[addr].str_addr = new_str_addr;
                    update_structural_address(new_str_addr, addr);
                    ps_cache_[addr].index_in_stream = ps_cache_[last_addr].index_in_stream + 1;
                }
                else{
                    assert(ps_cache_[last_addr].index_in_stream == MAX_STREAM_SIZE - 1);
                    ps_cache_[addr].str_addr = str_addr_counter;
                    ps_cache_[addr].index_in_stream = 0;
                    update_structural_address(str_addr_counter, addr);
                    str_addr_counter += MAX_STREAM_SIZE;
                }
                
                ps_cache_[addr].counter = 1;
                ps_cache_[addr].valid = 1;
            }

            training_unit[ip].last_addr = addr;   // finally, update last address to this


            // now issue prefetch requests
            uint64_t str_addr_stream_start = ps_cache_[addr].str_addr;
            for(uint32_t i = 0; i < prefetch_degree; i++){
                uint64_t str_addr_in_stream = str_addr_stream_start + i;
                if(sp_cache_.find(str_addr_in_stream) == sp_cache_.end() || sp_cache_[str_addr_in_stream].valid == 0){
                    continue;
                }
                prefetch_line(ip, addr, sp_cache_[str_addr_in_stream].phy_addr, FILL_L2, 0);
            }
        }

    }

    

}

void CACHE::l1d_prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr, uint32_t metadata_in)
{

}

void CACHE::l1d_prefetcher_final_stats()
{
    cout << endl;
}
