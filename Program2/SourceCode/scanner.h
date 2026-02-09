
/* Sudo code from lecture 3-- Gaining Understanding*/

/*
function Scanner() returns Token{

    while( s.peek() == blank){

        s.advance()

        if (s.EOF()){
            ans = $
        }else{
            if(s.peek()={0, 1,... 9}){
                ans = ScanDigits()
            }else{
                ch = s.advance()
                
                switch(ch){
                    case{a,b,  ..., z}-{i,f,p} remove these i, f, p
                        ans.type = id
                        ans.val = ch
                    case f:
                        ans.type = floatdcl
                    case i:
                        ans.type = intdcl
                    case p:
                        ans.type = print
                    case =
                        ans.type = assign
                    case +
                        ans.type = plus
                    case -
                        ans.type = minus
                    case default
                    LexicalError() 
                }
                return(ans)   
            }  
        }
    }
}
*/
/*
function ScanDigits() return token{
    tok.val = ""

    while(s.peek()= {0, 1, ...., 9}){
        tok.val = tok.val + s.advance()


        if(s.peek != "."){
            tok.type = inum
        }else{
            tok.type = fnum
            tok.val = tok.val + s.advance()
            while(s.peek()= {0, 1, ...., 9}){
                tok.val = tok.val + s.advance()
            }
        }
        return (tok)
    }
}
*/
