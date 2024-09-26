#include <string>
#include <tuple>
#include <array>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



/*
 *
Перечень команд:
 1 req1 a b (задать начальное значение = a и шаг = b для первой подпоследовательности);
 2 req2 a b (задать начальное значение = a и шаг = b для второй подпоследовательности);
 3 req3 a b (задать начальное значение = a и шаг = b для третьей подпоследовательности);
 4 get seq - выдавать в сокет каждому клиенту сгенерированную последовательность.

 Если в запросе любой из параметров (начальное значение и/или шаг) будет указан как = 0, то пропускаем и не обрабатываем последовательность;

 */

/*
 Usage/some test case:
 see also: make test
 { echo "  req2    2   4"; echo req3 3 2; echo get seq; \
   echo "req2 0 0"; echo get seq; 
 } | ./single

 */


using SeqType=int64_t;
struct Seq{
  SeqType val,step;
  SeqType next1; //val
  SeqType next2; //val+step


  Seq():
    val(0),step(0),
    next1(0),next2(0)
  {}

  //Note: validate after creation
  Seq(SeqType _val, SeqType _step):
    val(_val),step(_step),
    next1(_val),next2(_val+_step)
  {}

  bool IsValid() const {
    return val>0 && step>0;
  }

  //suggest IsValid
  SeqType GetNext(){
    SeqType ret = next1;
    next1 = next2;
    next2 = ret + next2;
    // 3 При переполнении счетчика подпоследовательность должна начинаться сначала;
    // Note: limit to 31/63 bit int
    if(next1<0){
      next1 = val;
      next2 = val+step;
    }
    return ret;
  }
};


enum Cmd{
  failed  = 0,
  get_seq = 1,
  req     = 2
};

auto process_string(std::string& s){
 //1 req1 a b (задать начальное значение = a и шаг = b для первой подпоследовательности);
 //4 get seq - выдавать в сокет каждому клиенту сгенерированную последовательность.

  char cmd[10];
  char args[10];
  int arg0,arg1,arg2;
  int got;

  got=sscanf(s.c_str(),"%9s %9s",cmd,args);
  if(got==2 && !strcmp(cmd,"get") && !strcmp(args,"seq")){
    printf("seq\n");
    return std::make_tuple(get_seq,0,0,0);
  }
  got=sscanf(s.c_str(),"%9s %d %d",cmd,&arg1,&arg2);
  if(got == 3 
    && !strncmp(cmd,"req",3)
    && cmd[3] >= '1' && cmd[3] <= '3'
    && cmd[4] == '\0'
  ){
    arg0=cmd[3]-'0';
    printf("req %d %d %d\n",arg0, arg1, arg2);
    return std::make_tuple(req, arg0, arg1, arg2);
  }
  printf("failed  %s %d %d %d %d\n",cmd, got, arg0 ,arg1, arg2);
  return std::make_tuple(failed,0,0,0);
}



int main(int , char* []){
  std::string ss;
  int res;
  std::array<Seq,3> se;
  do{
    char* s=NULL;
    size_t l=0;
    errno=0;
    res=getdelim(&s,&l,'\n',stdin);
    ss=s;
    free(s);
    if(res<0){
      if(errno){
	perror("getline: ");
	return 1;
      }
      printf ("getline EOF\n");
      return 0;
    }
    auto [res, seqno, start, step] = process_string(ss);
    if(res == req){
      se[seqno-1]={start,step};
    }
    if(res == get_seq){
      printf("%16.16llx\n",se[0].GetNext());
      printf("%16.16llx\n",se[0].GetNext());
      printf("%16.16llx\n",se[0].GetNext());
      printf("%16.16llx\n",se[0].GetNext());
      printf("%16.16llx\n",se[0].GetNext());
    }
  }while(res);
  //Seq se(0x7FFFFFF8,0x2);
  //Seq se(2,3);
  return 0;
}

