struct tuple {
    int a;
    int b;
    bool mem;
};

program CALCPROG {
    version CALCVERS {		  
        int ADD(tuple) = 1;  
        int MUL(tuple) = 2;   
        int INIT(int) = 3;    
    } = 1;			    
} = 0x30090949;			  
