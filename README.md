# LalaFrogKK
LalaFrogKK is a Nonogram solver developed by the team in the <a href=http://www.aigames.nctu.edu.tw/>CGI-Lab</a>  led by <a href=http://java.csie.nctu.edu.tw/~icwu/>Prof. I-Chen Wu</a>. The authors include Kan-Yueh Chen, Ching-Hua Kuo, Hao-Hua Kang, Der-Johng Sun, and I-Chen Wu. This version had won all the champions of all Computer Olympiad/TAAI/TCGA Nonogram tournaments <a href=http://java.csie.nctu.edu.tw/~icwu/honors.html>since TAAI 2011 and till Computer Olympiad 2015</a> (around when this version is published). 

This program was written based on the algorithm described in the following paper:

[1] I-Chen Wu, Der-Johng Sun, Lung-Ping Chen, Kan-Yueh Chen, Ching-Hua Kuo, Hao-Hua Kang, Hung-Hsuan Lin, "An Efficient Approach to Solving Nonograms," IEEE Transactions on Computational Intelligence and AI in Games, vol.5, no.3, pp.251~264, Sept. 2013

The following explains some settings in "config.h":
- P_SIZE is the width as well as the height of puzzles, 25 in the tournaments. 
- NUM_OF_QUESTIONS is the total number of puzzles to be solved, 1000 in the tournaments. 
- MODE is set to 1 for recent tournaments, which means to solve from the first puzzle to the last one with only finding one solution. Other settings of MODE are for previous rules of tournaments. 
- INPUT_FILE is the name of input file, where all the puzzles to be solved are stored in. 
- OUTPUT_FILE is the name of output file, where all the solutions of the puzzles are written to. 
- LOG_FILE is the name of log file, where the time spent to solve each puzzle are recorded. 

The following is the format of input and output file: 
- There is a symbol "$" before the puzzle number. The description/solution of the puzzle starts from the next line. 
- There are P_SIZE*2 lines of clues (puzzle description), the former P_SIZE are clues at the top end from up to down. The later P_SIZE are clues at the left end from left to right. 
- The solutions are stored by P_SIZE*P_SIZE matrices. A "0" means to leave a cell blank while a "1" means to color a cell black. 
- All the numbers of clues and solutions are separated by TAB. 
<br>For the following 4x4 puzzle<br>
<pre>
vvvv
&nbsp;vv&nbsp;
v&nbsp;vv
vv&nbsp;&nbsp;
</pre>
The input is <br>
<pre>
1	2
2	1
3
1	1
4
2
1	2
2
</pre>
The output is <br>
<pre>
1	1	1	1
0	1	1	0
1	0	1	1
1	1	0	0
</pre>

Bugs may exist in the code. For example, the puzzle in PuzzleCannotSolve.txt cannot be solved by this version of LalaFrogKK. 

Since no one in our lab maintains the code anymore, we may not be able to answer your questions. However, any comments are welcome and sent to nonogram@aigames.nctu.edu.tw. 

CGI Lab<br>
Department of Computer Science<br>
National Chiao Tung University<br>
Hsinchu, Taiwan
