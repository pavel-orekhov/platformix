#include <stdio.h>
#include <string>



/*
 *
Перечень команд:
 1 req1 a b (задать начальное значение = a и шаг = b для первой подпоследовательности);
 2 req2 a b (задать начальное значение = a и шаг = b для второй подпоследовательности);
 3 req3 a b (задать начальное значение = a и шаг = b для третьей подпоследовательности);
 4 get seq - выдавать в сокет каждому клиенту сгенерированную последовательность.

 Если в запросе любой из параметров (начальное значение и/или шаг) будет указан как = 0, то пропускаем и не обрабатываем последовательность;

 */


using SeqType=int64_t;
struct Seq{
  SeqType val,step;
  SeqType next1; //val
  SeqType next2; //val+step


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



int process_string(std::string& s){

  //sscanf(s.c_str(),"%s",cmd);
  return 0;
}



int main(){
  Seq se(0x7FFFFFF8,0x2);
  //Seq se(2,3);
  printf("%16.16llx\n",se.GetNext());
  printf("%16.16llx\n",se.GetNext());
  printf("%16.16llx\n",se.GetNext());
  printf("%16.16llx\n",se.GetNext());
  return 0;
}

