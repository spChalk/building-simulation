
#include <iostream>
#include <cstdio>

#include "classes.h"

using namespace std;

#if 0 //TODO: Remove before finalisation
void debug_cout(visitor *vst){             
  std::cout << "<<<<<<<<<<<<<<" << " F: " << vst->get_floor() << "  O: " 
  << vst->get_office_num() << "  P: " << vst->get_priority() << std::endl; }

#define DEBUG(X) debug_cout(X);
#endif
/* ============================================||  V I S I T O R   F U N C T I O N S  ||============================================ */ 

visitor::visitor(const short fl, const short off, const unsigned int pr) 
: floor(fl), office_num(off), priority(pr), is_satisfied(false) {}

void visitor::set_satisfaction(const bool sat) { is_satisfied = sat; }

bool visitor::get_satisfaction() { return is_satisfied; }

unsigned int visitor::get_priority() { return priority; }

short visitor::get_office_num() { return office_num; }

short visitor::get_floor()  { return floor; }


/* ============================================||  W A I T I N G    R O O M    F U N C T I O N S  ||============================================ */ 

waiting_room::waiting_room() : curr(0) {}

waiting_room::~waiting_room() { std::cout << "End of waiting people!\n"; }

void waiting_room::enter(visitor* vst) {
  curr++;
  visitors.push(vst);
}

visitor* waiting_room::exit() {
  visitor* vst=visitors.front();
  visitors.pop();
  curr--;
  return vst;
}

queue<visitor*> waiting_room::get_vst(void) { return visitors; }

unsigned int waiting_room::get_curr() { return curr; }


/* ============================================||  G R O U N D   L E V E L   F U N C T I O N S  ||============================================ */ 

ground_level::ground_level (const int Ng, building *const bldg)
: cap(Ng), curr(0), bld(bldg) 
{
  wr  = new waiting_room;
  std::cout << "The Entrance has been created!\n";
}

ground_level::~ground_level(){ 
  delete wr;
  std::cout << "End of service!\n";
}

waiting_room* ground_level::get_wr(void) { return wr; }

unsigned int ground_level::get_cap() { return cap; }

unsigned int ground_level::get_curr() { return curr; }

bool ground_level::enter(visitor* vst) {
  if (get_curr() == get_cap())
    return false;
  curr++;
  wait(vst);
  return true;
}

void ground_level::wait (visitor* vst) { wr->enter(vst); }

void ground_level::exit(visitor *vst){
  --curr;
  bld->exit(vst);
}


/* ============================================||  O F F I C E   F U N C T I O N S  ||============================================ */

office::office(unsigned int No,short num) 
: number(num), cap(No)
{ std::cout << "Office #" << number << " has been created\n"; }

office::~office() { std::cout << "End of the work!\n"; }

bool office::is_empty() { return (visitors.size() == 0); } 

unsigned int office::get_cap(){ return cap; }

bool office::enter(visitor *vst) {
  if (visitors.size() == (unsigned int)(get_cap())){
    std::cout << "Please, wait outside for entrance in the office. Your priority is: " << vst->get_priority() << endl;
    return false;
  } else {
    visitors.push(vst);
    std::cout << "Entering office #" << number << endl;
    return true;
  }
} 

visitor *office::exit() { 
  visitor *vst = visitors.front();
  vst->set_satisfaction(true);
  visitors.pop();
  return vst;  
}


/* ============================================||  F L O O R   F U N C T I O N S  ||============================================ */ 

floor::floor(unsigned int Nf, unsigned int No, short num) 
: number(num), cap(Nf), curr(0)
{
  wr=new waiting_room;
  off=new office*[10];
  for (short i = 0; i < 10; i++) off[i]=new office(No,i+1);
}

floor::~floor() {
  delete wr;
  for (short i = 0; i < 10; i++)
    delete off[i];
  delete[] off;
  std::cout<<"End of service!\n";
}

waiting_room* floor::get_wr(void) { return wr; }

office *floor::get_office(short off_n){ return off[off_n-1]; } //[Harry] args from 1-10

unsigned int floor::get_cap() { return cap; }

unsigned int floor::get_curr() { return curr; }

bool floor::enter(visitor* vst) {
  if (get_curr() < get_cap()) {
    if (get_office(vst->get_office_num())->enter(vst) == false)  // An [den] xwraei sto grafeio
      wr->enter(vst);
    curr++;
    return true;
  }
  std::cout << "Sorry, floor number " << number << " is full." << endl;
  std::cout << "Your priority is: "   << vst->get_priority()  << endl;
  return false;
}

// Randomly chooses and returns a person from an office
// Please care about an empty floor and a non-empty WR
visitor *floor::exit() {
  --curr;
  while(1){
    short i = rand() % 10 + 1;
    if( get_office(i)->is_empty() == false ) {
      return get_office(i)->exit();
    }
  }
}


/* ============================================||  E L E V A T O R   F U N C T I O N S  ||============================================ */ 

elevator::elevator(unsigned int Nl,unsigned int l_circl, floor **fl_arr,ground_level* gr_lvl)
: cap(Nl), curr_fl(0), fl(fl_arr), grl(gr_lvl), curr(0), crcl_rem(l_circl) {}

elevator::~elevator() { std::cout << "No more ups and downs!\n"; }

bool elevator::enter(visitor* vst) {
  if (get_curr() < get_cap()) {
    visitors.push(vst);
    curr++;
    std::cout << "Visitor in the lift!" << endl;
    return true;
  }
  std::cout << "You are not allowed to enter!\n" << "Your priority is: "
            << vst->get_priority() << endl;
  return false;
}

void elevator::exit(visitor *vst) {
  --curr;
  grl->exit(vst);
}

// selects visitors ready2leave from the floor and puts them in the elevator
void elevator::stop_down() {
  for (short fl_num = 4; fl_num >= 1; --fl_num)
  {
    floor *flr = fl[fl_num-1];
    std::cout << "Going down to floor " << fl_num  << std::endl;
    unsigned int sel = get_cap() - get_curr();
    for (unsigned int i = 0; i < sel && (flr->get_curr() != flr->get_wr()->get_curr()); ++i)
    {
      ++curr;
      visitors.push(flr->exit());
    }
  }
}

void elevator::stop_up() {
  for (short cur_fl = 1; cur_fl <= 4; ++cur_fl)
  {
    std::cout << "Going up to floor " << cur_fl << std::endl;
    // this is done so that ppl avoid being stuck eternally in the wr
    for (unsigned int i = 0, max = fl[cur_fl-1]->get_wr()->get_vst().size(); i < max; ++i)
    {
      visitor *vst = fl[cur_fl-1]->get_wr()->exit();
      office *off =fl[cur_fl-1]->get_office(vst->get_office_num());
      if (off->enter(vst) == false) // if they get rejected
        fl[cur_fl-1]->get_wr()->enter(vst);
    }

    for (unsigned int i = 0, max = visitors.size(); i < max; ++i)
    {
      visitor *vst = visitors.front();
      if (!(vst->get_floor() == cur_fl && fl[cur_fl-1]->enter(vst)))   // if correct floor -> try to enter
        visitors.push(vst);
      else  curr--;
      visitors.pop();
    }
  }
}

void elevator::operate() {
  while (crcl_rem--) {
    while (get_curr() < get_cap() && grl->get_wr()->get_curr()){ // while not cap + has ppl w8ting
      visitor* vst=grl->get_wr()->exit();
      if(!enter(vst))                             // val'tous olous apo isogeio
        grl->get_wr()->enter(vst);                // Stin periptwsh pou den mpei sto asanser, 3anavalton mesa sto grl 
    }                                               
    stop_up();
    stop_down();
    empty_all();
  }
}

// calls elevator::exit on every satisfied client inside the lift
void elevator::empty_all() {
  for (unsigned int i = 0, max = visitors.size();  i < max; i++) 
  { 
    visitor *vst = visitors.front();
    if (vst->get_satisfaction() == false)
      visitors.push(vst);  
    else 
      exit(vst);
    
    visitors.pop();
  }
}

unsigned int elevator::get_cap() { return cap; }

unsigned int elevator::get_curr() { return curr; }


/* ============================================||  B U I L D I N G   F U N C T I O N S  ||============================================ */ 

building::building(unsigned int N, unsigned int Nf, unsigned int Ng, unsigned int No, unsigned int Nl, unsigned int l_circl)
: cap(N), curr(0) {
  std::cout << "A new building is ready for serving citizens!\n\n";
  gr_lvl = new ground_level(Ng, this);
  std::cout << "A Floor has been created with number 0!\n";
  fl = new floor*[4];
  for (short i = 0; i < 4; i++) {
    fl[i] = new floor(Nf, No, i+1);
    std::cout << "A Floor has been created with number " << i+1 << "!\n";
  }
  el = new elevator(Nl, l_circl, fl,gr_lvl);
  std::cout << "A lift has been created!\n";
}

building::~building() {
  delete el;
  for (short i = 0; i < 4; i++)
    delete fl[i];
  delete[] fl;
  delete gr_lvl;
  std::cout<<"Service not available any longer. Go elsewhere!\n";
}

void building::enter (visitor* vst) {
  if ((curr == cap) || gr_lvl->enter(vst) == false)
    std::cout << "Please, come tomorrow. P: " << vst->get_priority() << std::endl;
  else {
    curr++;
    std::cout << "A new customer wants to go to Floor " << vst->get_floor()
              << " and Office " << vst->get_office_num() << " with Priority "
              << vst->get_priority() << std::endl; 
  }
}

void building::exit(visitor *vst) {
  --curr;
  std::cout << "\n\"I can't believe I finished!\" - " 
            << "F: " << vst->get_floor() << "  O: " << vst->get_office_num()
            << "  P: " << vst->get_priority() << " " << std::endl;
}

ground_level* building::get_gr_lvl(void) { return gr_lvl; }

elevator* building::get_elevator(void) { return el; }

/* ========================================================||  END OF FILE  ||======================================================== */ 