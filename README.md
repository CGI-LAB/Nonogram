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

Any comments are welcome and sent to nonogram@aigames.nctu.edu.tw. 

CGI Lab<br>
Department of Computer Science<br>
National Chiao Tung University<br>
Hsinchu, Taiwan
