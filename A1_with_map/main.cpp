// Name: Dumitrica Flavia

//pre-processor directives:
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <ctime>
#include <random>
#include <cstdlib>
#include <string>
#include <chrono>
#include <map>
#include <sstream>
using namespace std;

//Initializing pseudo-random number generator
std::mt19937 gen(time(0));
std::uniform_int_distribution<> dis(1, 1000);

//Global constants
int const MAX_NUM_OF_THREADS = 6;
int const NUM_OF_SAMPLES = 50;


//Global variables
int counter_ts=0;
int counter_ps=0;
int counter_cs=0;
std::map<std::thread::id, int> threadIDs;

//Function which atomizes the printing action
 void printing (std::string str){
     std::mutex m; //mutex
     std::lock_guard<std::mutex> lg(m);
     cout<<str<<endl;
 }


//Abstract base class that models a sensor
class Sensor {
 public:

 Sensor(string& type) : sensorType(type) {}

 //Declare a pure virtual method to be overridden by derived classes
 virtual double getValue() = 0;

 //Declare non-virtual method
 string getType() {
    //Returns the type of Sensor
    return sensorType;
 }

 //Declare any instance variable(s)
 string sensorType= "";

};


//Derived class for temperature sensor
class TempSensor : public Sensor {
 public:

 TempSensor (string& s) : Sensor(s) {}

 //Return a random value of ambient temperature between 10 and 30
 virtual double getValue() {
    return (dis(gen) % 21 + 10.0);
 }

};


//Derived class for pressure sensor
class PressureSensor : public Sensor {
 public:

 PressureSensor (string& s) : Sensor(s) {}

//Return a random value of pressure between 95 and 105
 virtual double getValue() {
    return (dis(gen) % 11 + 95.0);
 }

};

//Derived class for capacitive sensor
class CapacitiveSensor : public Sensor {
 public:
 //Constructor
 CapacitiveSensor (string& s) : Sensor(s) {}

 //Return a random value of capacitance between 1 and 5
 virtual double getValue() {
    return (dis(gen) % 5 + 1.0);
 }

};


class BC {
public:
// constructor initialises a vector of Sensor pointers that are passed in by reference
BC(std::vector<Sensor*>& sensors): theSensors(sensors) {}


void requestBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    while (lock){
        stringstream s4;
        s4<< "Bus Controller is locked, so thread "<<threadIDs[std::this_thread::get_id()]<<" is about to suspend..."<<endl;
        printing(s4.str());
        vc.wait(ul);
    }
    lock= true;
}

double getSensorValue(int selector) {
    return (*theSensors[selector]).getValue();
}

string getSensorType(int selector) {
    return (*theSensors[selector]).getType();
}

void releaseBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    lock=false;
    vc.notify_one();
}
private:
 bool lock = false; //'false' means that the BC is not locked
 std::vector<Sensor*>& theSensors; //Reference to vector of Sensor pointers
 std::mutex BC_mu; //Mutex
 std::condition_variable vc; // Condition variable

};

//"run" function executed by each thread
 void run(BC& theBC, int idx) {
    int i=0;
    int sel=0;
    double val;
    string typ;
    string t_s= "temperature sensor";
    string p_s= "pressure sensor";
    string c_s= "capacitive sensor";

    //Implementing a map
    std::mutex map_mu; //declare a mutex
    std::unique_lock<std::mutex> map_locker(map_mu); //lock the map via the mutex
    threadIDs.insert(std::make_pair(std::this_thread::get_id(), idx));
    map_locker.unlock(); // unlock the map



    for (i=0; i< NUM_OF_SAMPLES; i++) {
        val=0;
        typ="";
        sel=0;
        // request use of the BC
        theBC.requestBC();
        stringstream s1;
        s1<< "Bus Controller locked by thread "<<idx<<endl;
        printing(s1.str());

        // generate a random value between 0 and 2, and use it to
        // select a sensor and obtain a value and the sensor's type
        sel = dis(gen) % 3;
        val= theBC.getSensorValue(sel);
        typ= theBC.getSensorType(sel);

        //print out sensor type, sample value and thread id
        stringstream s2;
        s2<<" sample value from thread "<<idx<<" of type "<<typ<<" is equal to "<<val<<endl;
        printing(s2.str());

        // increment counter for the chosen sensor
        if(typ== t_s){counter_ts++;}

        if(typ== p_s){counter_ps++;}

        if(typ== c_s){counter_cs++;}

        // release BC
        theBC.releaseBC();
        stringstream s3;
        s3<< "Bus Controller UNlocked by thread "<< idx<<endl;
        printing(s3.str());

        // delay for random period between 0.001s and 0.01s:
        std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 11) + 1));
    }

}

int main() {
 //declare a vector of Sensor pointers
 std::vector<Sensor*> sensors;

 //initialise each sensor and insert into the vector
 string ts = "temperature sensor";
 sensors.push_back(new TempSensor(ts)); //push_back is a vector method
 string ps= "pressure sensor";
 sensors.push_back(new PressureSensor(ps));
 string cs= "capacitive sensor";
 sensors.push_back(new CapacitiveSensor(cs));

 // Instantiate the BC
 BC theBC(std::ref(sensors));

 //instantiate and start the threads
 std::thread t[MAX_NUM_OF_THREADS]; //array of threads
 for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
 //launch the threads
 t[i] = std::thread(run, std::ref(theBC), i);
 }

 //wait for the threads to finish
 for (int i = 0; i< MAX_NUM_OF_THREADS; i++) {
 t[i].join();
 }
 cout << "All threads terminated" << endl;

//print out the number of times each sensor was accessed
 cout<< "The temperature sensor was accessed "<< counter_ts<<" times."<< endl;
 cout<< "The pressure sensor was accessed "<< counter_ps<<" times."<< endl;
 cout<< "The capacitive sensor was accessed "<< counter_cs<<" times."<< endl;

 return 0;
} // end of main
