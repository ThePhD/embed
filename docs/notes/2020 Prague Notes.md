#depend - We would like to have this feature in C++(Something) and spend time figuring out the details.
SF  F  N  A  SA
14 13  2  0   0
Consensus: Do more work.

#depend <foo/**> - We want recursive globs (recursively searching through directories) for #depend.
SF  F  N  A SA
 2  3  9 12  4
Consensus: Do not want.
A: Complexity.

It should be mandatory that EVERY file for std::embed is specified by a #depend.
SF  F  N  A SA
 4 12  5  6  3
Consensus: Split, no consensus. Add why/why not.
SF: This MUST exist. Both compiler and build system authors. (Implementers.)
SA: Can make user experience sad face for common case. Build system should scream at you for making the mistake.


#depend should form a Virtual File System / State that constrains the search and should be passed to std::embed.
SF  F  N  A  SA
 3 11  7  2   2
Consensus: Do it.
SA: Hell to implement.


Make std::embed ill-formed inside of a module interface (with a plan to revisit later).
SF  F  N  A SA
 4  2  7  1  1
SA: Modules are important we should make sure it interacts well with modules (figure it out now).
SF: How does this work with #depend ?
SF: std::embed is basically a #include -- why would we want it in interface? Just focus on getting feature working and doing it well.
A: Jumping the gun. Space needs more exploration.
N: We are highly undecided - need to answer more questions (especially about Modules).
