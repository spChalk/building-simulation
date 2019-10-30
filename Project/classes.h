#pragma once

#include <iostream>
#include <cstdio>
#include <queue>

using namespace std;

/* Declarations */
class visitor;
class waiting_room;
class floor;
class office;
class ground_level;
class building;
class elevator;
 
/* =========================||  C L A S S E S  ||========================= */
class visitor 
{
  int floor;
  int office_num;
  int priority; 
  bool is_satisfied;
public:
  visitor(int fl, int off);
  void set_priority(int);
  int get_priority();
  int get_office_num();
  int get_floor();
  void set_satisfaction(bool);
  bool get_satisfaction(); 
};

class waiting_room
{
  int curr;
  queue<visitor*> visitors;  
public:
  waiting_room();    /* Waiting room does not have maximum capacity */
  ~waiting_room();
  void enter(visitor*);
  visitor* exit(); 
  int get_curr();
  queue<visitor*> get_vst();
};

class ground_level
{
  int cap;
  int curr;
  building* bld;  //[Harry] Questionable alla to kratame & vlepoume   // [Spiros] Looks like to kratame telika
//  elevator* el; // [Harry] if bugs, enable this
  waiting_room* wr;
public:
  ground_level(int Ng, building *);
  ~ground_level();
  bool enter(visitor*); //[Harry] TODO: bool + capacity check 
  void exit(visitor*); 
  void wait(visitor*); //metaferei ton visitor sto wr 
  int get_cap();
  int get_curr();
  waiting_room* get_wr();
};

class office
{
  int number;
  int cap;
  int total; // total visitors, used to prioritize ppl (bank-style)
//   floor** fl;
//   elevator* el;       // [Spiros] Mallon xreiazetai kai edw to elevator gt sto exit paei kateu8eian sto elev
  queue<visitor*> visitors;
public:
  office(int No, int num);
  ~office();
  bool enter(visitor *);
  visitor *exit();
  int get_cap(); // opt?
  bool is_empty();  //[Harry] used in elev::stop_down
};

class floor
{
  int number; // [1,4]
  int cap; //opt ? 
  int curr;//opt ?
  waiting_room* wr;
  //elevator* el;
  office** off;
public:
  floor(int Nf,int No);
  ~floor();
  bool enter(visitor *);    // [Spiros] bool enter klassika //[Harry] :heart_eyes:
  visitor *exit();
  int get_cap();
  int get_curr();
  waiting_room* get_wr();
  office *get_office(int office_n); //[Harry] added this one used on elevator::stop_down
};

class elevator
{
  int total; // [Harry] used to prioritize ppl as usual
  int cap;
  int curr_fl;
  floor** fl;
  ground_level* grl;
  int curr;
  int crcl_rem;   // circles remaining // ousiastika termatizei th diadikasia
  queue<visitor*> visitors;
  bool enter(visitor*);     
  void exit(visitor*);   
  void empty_all(); 
  void stop_up();
  void stop_down();
public:                          
  elevator(int Nl, int lc, floor **,ground_level*);    
  ~elevator();
  void operate();
  int get_cap();
  int get_curr();
};

class building
{
  int cap;  /* capacity */
  int curr; /*  current ppl inside */
  ground_level* gr_lvl;
  floor** fl;  // Floor pointer (create floors dynamically during construction)
  elevator* el;
public:
  building(int N, int Nf, int Ng, int No, int Nl, int lc);  
  ~building();                                
  void enter(visitor *);
  void exit(visitor *);
  elevator* get_elevator();
  ground_level* get_gr_lvl();
};