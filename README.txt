-------------
README
-------------
included:
a3search.cpp
makefile
this README

-------------
Datat Structure
-------------
specification: 
	block :a two character combination, e.g. "ap" is a block
        minimum length of a pattern: minPL

(1) SHIFT TABLE
    it is an int type array,  size is 256*256
    the index is the hash value of a block
    the content is the shift distance of each block

(2) HASH TABLE (build block(pattern[minPL-2]pattern[minPL-1]) -> pattern relation ship)
        HASH TABLE ---------------->pattern_pos_list ------> pattern_list
size    256*256            |       unique block number   |   pattern number
type    int[]              |       vector<int>           |   vector<int>
content index of block in  |  the start position of a    |   patterns sorted by block's
	pattern_pos_list   |  block's responding patter  |   hash value

(3) PREFIX TABLE (it is used for quickly sifting impossible pattern)
    I used map<string, int> to build the relationship between a pattern and its prefix block's hash value.

--------------
Algorithm Analysis
--------------
WM multiple pattern matching algorithm
The shift distance is decided by the SHIFT TABLE. Once SHIFT[key]=0, it means that the pattern is likeyly to be matched. Then we use PREFIX TABLE to deduce the scope to increase the efficiency. After checking these, we shift the text pointer to the next. If SHIFT[key]!=0, we just shift the text pointer in the distance of SHIFT[key].

--------------
Design issues and summary
--------------
If just a single pattern to match, we can use Sunday algorithm. But if we need to search multiple patterns, WM algorithm offers three tables to speed up match mode. And the data structure of these three tables could also affect the efficiency significantly.
Furthermore,  how long the distance that WM can jump is decided by the shortest pattern; in some situation, WM may degrade. For example, if one pattern is very short and other patterns are very long and with a long and same prefix(e.g.only the last character is different). So jump distance is short and checking pattern loop will also take many time. 

