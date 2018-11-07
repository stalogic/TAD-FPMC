# TAD-FPMC

### Train model
here are code used in train step.
1) random.h
2) matrix.h
3) tensor.h
4) pitf.h
5) train.cpp

use "g++ -o train train.cpp" to compile the code.

The train program will read train.txt and then generate 6 parameter file (l_p.txt, l_t.txt, l_u.txt, p_l.txt, t_l.txt, u_l.txt).
The format of train.txt 

|user_id	|	time | 	current_POI_category	| delta_T	 | next_POI_category |
|---------|---------|---------|---------|---------|
|1 |	3 |	61 |	47.11	| 160 |
|1 |	3 |	61|	47.11|	147|
|1|	5|	147|	0.03	|108|


### Predict 
run predictCate.py to generate predict next_POI_category, and use cateprecision.py to get precision.
