// REQUIREMENTS SUMMARY

Virtual Memory requires the system to maintain a MAPPING TABLE for each page in memory.
-> When the process references an address, use the table to map virtual addr to real location in machine memory.
-> If referenced page is not loaded, place it in the memory frame
   If free frame exists, use that frame
   If all frames are used, make more space by replacing content of existing frame

Two replacement strategies:
1. LRU = Replace oldest accessed page
2. FIFO = Replace first loaded page

Aim of assignment:
1. Build simple sim of a system that can map and manage VM to machine memory
2. The sim can measure how the system behaves if we:
   -> change #pages
   -> change #frames
   -> change replacement strategy

Notes:
-> We dont represent pages/frames in our system, nor 'execute' programs
-> We provide 'data structures for the page table' and a 'minimal rep of memory pages'
-> This gives enough to read a seq of page references and measure how the table might behave in a real implementation

/* PSEUDOCODE */

void processExecution(void) {
	Page = ProcessReferenceAddr(addr)
	if (Page is !loaded) {
		placeInMemoryFrame(page);
	}
}

void placeInMemoryFrame(page) {
	if (frame exists) {
		loadPageToFrame(page)
	} else {
		findVictim()
	}
}

// Walkthrough of code





