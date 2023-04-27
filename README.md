# CS230_Project

Team: IntelLigent Prefetchers

### Team Members
- Ananth Krishna Kidambi : 210051002
- Josyula Venkata Aditya : 210050075
- Guramrit Singh : 210050061

## IPCP + Page Aware Prefetching

This github repository is made as a part of CS230, 2023 course project. The code present in this repository is based on ChampSim, a trace based simulator. IPCP is an Instruction Pointer based prefetcher(check this link: https://dpc3.compas.cs.stonybrook.edu/pdfs/Bouquet.pdf). We propose an enhancement to this prefetcher by introducing **Page Aware Prefetching** i.e. we prefetch data placed in different pages as well. This might also help us in prefetching page mappings into the TLBs that would be used in the future. 

link to traces: 
- https://utexas.app.box.com/s/2k54kp8zvrqdfaa8cdhfquvcxwh7yn85/folder/132804598561
- https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu/
- https://drive.google.com/file/d/1qs8t8-YWc7lLoYbjbH_d3lf1xdoYBznf/view
- https://www.dropbox.com/sh/xs2t9y4cuqlgrlp/AACpzGOj6BcSB-BUolGaBjbta?dl=0

link to ChampSim:
https://github.com/casperIITB/ChampSim

## High level overview of the implementation:
In IPCP, we issue prefetches according to strides when using the constant stride and stream prefetches. Let the base physical address used for issuing prefetches be *p* and the virtual address corresponding to *p* be *v*. In IPCP, we increment *p* by *stride* and prefetch this address until we hit the prefetch degree. When *p+i\*(stride)* crosses the page boundary, IPCP stops prefetching. PAP tries to not stop prefetching at this stage by issuing a translation request to the DTLB for the virtual address *v+i\*(stride)*. After we get the physical address corresponding to this virtual address, we again start prefetching until we prefetch the remnant of *prefetch_degree* for the given base address.

