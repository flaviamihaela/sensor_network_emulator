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

//Initializing the pseudo-random number generator
std::mt19937 gen(time(0));
std::uniform_int_distribution<> dis(1, 1000);

//Global constants
int const MAX_NUM_OF_THREADS = 20;
int const NUM_OF_LINKS = 2;
int const NUM_OF_SAMPLES = 50;
string const t_s= "temperature sensor";
string const p_s= "pressure sensor";
string const c_s= "capacitive sensor";


// Global variables
int counter_ts=0;
int counter_ps=0;
int counter_cs=0;

// Map for thread Ids
std::map<std::thread::id, int> threadIDs;

// Function which atomizes the printing action:
 void printing (std::string str){
     std::mutex m; //mutex
     std::lock_guard<std::mutex> lg(m);
     cout<<str<<endl;
 }


// Utility class to store sensor data
class SensorData{
 public:

 SensorData(string type) : sensor_type(type) {}

 string getSensorType() {
     return sensor_type;
 }
 std::vector<double> getSensorData() {
     return sensor_data;
 }
 void addData(double newData) {
     sensor_data.push_back(newData);
 }
 private:
 string sensor_type="";
 std::vector<double> sensor_data;
};

class Receiver{
public:

 Receiver () { }

 //Receives a SensorData object:
 void receiveData(SensorData sd) {
    std::unique_lock<std::mutex> Rec_ul(mtx);
    std::vector<double> sensdata = sd.getSensorData();
    if (sd.getSensorType() == t_s){
        temp_data.insert(temp_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == p_s){
        press_data.insert(press_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == c_s){
        cap_data.insert(cap_data.end(), sensdata.begin(), sensdata.end());
    }
 }

 // Prints out all data for each sensor:
 void printSensorData () {
    cout<<"temperature data:"<<endl;
    for (vector<double>::iterator it = temp_data.begin(); it != temp_data.end(); ++it){
        cout<<*it<<endl;
    }

    cout<<"pressure data:"<<endl;
    for (vector<double>::iterator it = press_data.begin(); it != press_data.end(); ++it){
        cout<<*it<<endl;
    }

    cout<<"capacitance data:"<<endl;
    for (vector<double>::iterator it = cap_data.begin(); it != cap_data.end(); ++it){
        cout<<*it<<endl;
    }
 }
 private:
 std::mutex mtx; //mutex
 std::vector<double> temp_data; //vector stores temperature data received from thread
 std::vector<double> press_data; //vector stores pressure data received from thread
 std::vector<double> cap_data; //vector stores capacitance data received from thread
};

class Link{
 public:

 Link (Receiver& r, int linkNum) : inUse(false), myReceiver(r), linkId(linkNum){}

 //check if the link is currently in use
 bool isInUse() {
    return inUse;
 }
 //set the link status to busy
 void setInUse() {
    inUse = true;
 }
 //set the link status to idle
 void setIdle() {
    inUse = false;
 }
 //write data to the receiver
 void writeToDataLink(SensorData sd) {
    myReceiver.receiveData(sd);
}
 //returns the link Id
 int getLinkId() {
    return linkId;
 }
 private:
 bool inUse;
 Receiver& myReceiver; //Receiver reference
 int linkId;
};

class LinkAccessController {
public:
 //Constructor
 LinkAccessController(Receiver& r): myReceiver(r), numOfAvailableLinks(NUM_OF_LINKS)
 {
    for (int i = 0; i < NUM_OF_LINKS; i++) {
        commsLinks.push_back(Link(myReceiver, i));
  }
 }

 //Request a comm's link: returns a reference to an available Link.
 //If none are available, the calling thread is suspended.
 Link& requestLink() {
 int linkNum;
 std::unique_lock<std::mutex> LAC_ul(LAC_mu);
 while (commsLinks[0].isInUse() && commsLinks[1].isInUse()){
    stringstream s6;
    s6<< "Thread "<<threadIDs[std::this_thread::get_id()]<<" is suspended on links"<<endl;
    printing(s6.str());
    LAC_cv.wait(LAC_ul);
 }
 for (Link& elem : commsLinks){
    if (!elem.isInUse()) {
        elem.setInUse();
        linkNum = elem.getLinkId();
        break;
    }
 }
 return std::ref(commsLinks[linkNum]);

 }

 //Release a communication link
 void releaseLink(Link& releasedLink) {
 std::unique_lock<std::mutex> LAC_ul(LAC_mu);
 releasedLink.setIdle();
 LAC_cv.notify_one();
 }

private:
 Receiver& myReceiver; //Receiver reference
 int numOfAvailableLinks;
 std::vector<Link> commsLinks;
 std::mutex LAC_mu; //mutex
 std::condition_variable LAC_cv;

};

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
 //Constructor
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

 CapacitiveSensor (string& s) : Sensor(s) {}

 //Return a random value of capacitance between 1 and 5
 virtual double getValue() {
    return (dis(gen) % 5 + 1.0);
 }

};


class BC {
public:
//constructor initialises a vector of Sensor pointers that are passed in by reference
BC(std::vector<Sensor*>& sensors): theSensors(sensors) {}


void requestBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    while (lock){
        std::stringstream s;
        s<< "Bus Controller is locked so thread "<<threadIDs[std::this_thread::get_id()]<<" is about to suspend..."<<endl;
        printing(s.str());
        vc.wait(ul);
    }
    lock= true;
    std::stringstream s1;
    s1<< "Bus Controller locked by thread "<<threadIDs[std::this_thread::get_id()]<<endl;
    printing(s1.str());
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
    std::stringstream s3;
    s3<< "Bus Controller UNlocked by thread "<<threadIDs[std::this_thread::get_id()]<<endl;
    printing(s3.str());
}
private:
 bool lock = false; //'false' means that the BC is not locked
 std::vector<Sensor*>& theSensors; //Reference to vector of Sensor pointers
 std::mutex BC_mu; //Mutex
 std::condition_variable vc; // Condition variable

};

//This function is executed by each thread:
 void run(BC& theBC, int idx, LinkAccessController& lac) {
    int i=0;
    int sel;
    double val;
    string typ;
    SensorData t_s_data(t_s);
    SensorData p_s_data(p_s);
    SensorData c_s_data(c_s);

    //Implementing a map
    std::mutex map_mu; //declare a mutex
    std::unique_lock<std::mutex> map_locker(map_mu); //lock the map via the mutex.
    threadIDs.insert(std::make_pair(std::this_thread::get_id(), idx));
    map_locker.unlock(); // unlock the map

    for (i=0; i< NUM_OF_SAMPLES; i++) {
        val=0;
        typ="";
        sel=0;
        // request use of the BC:
        theBC.requestBC();

        // generate a random value between 0 and 2, and use it to
        // select a sensor and obtain a value and the sensor's type:
        sel = dis(gen) % 3;
        val= theBC.getSensorValue(sel);
        typ= theBC.getSensorType(sel);

        //print out sensor type, sample value and thread id
        std::stringstream s2;
        s2<<" sample value from thread "<<idx<<" of type "<<typ<< " is equal to "<<val<<endl;
        printing(s2.str());

        // increment counter for the chosen sensor
        if(typ== t_s){
                counter_ts++;
                t_s_data.addData(val);
        }

        if(typ== p_s){
                counter_ps++;
                p_s_data.addData(val);
        }

        if(typ== c_s){
                counter_cs++;
                c_s_data.addData(val);
        }

        // release the BC:
        theBC.releaseBC();

        // delay for random period between 1 and 10 milliseconds:
        std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));

    } 

    Link* link = &lac.requestLink();
    std::stringstream s4;
    s4<< "Thread "<<idx<<" successfully acquired link "<<link->getLinkId()<<endl;
    printing(s4.str());
    link-> writeToDataLink(t_s_data);
    link-> writeToDataLink(p_s_data);
    link-> writeToDataLink(c_s_data);
    lac.releaseLink(*link);
    std::stringstream s5;
    s5<<"Thread "<<idx<<" successfully released link "<<link->getLinkId()<<endl;
    printing(s5.str());

    // delay for random period between 1 and 10 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));

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

 // Instantiate BC
 BC theBC(std::ref(sensors));

 // Instantiate Receiver
 Receiver theReceiver;

 // Instantiate Link Access Controller
 LinkAccessController LAC(theReceiver);

 //instantiate and start threads
 std::thread t[MAX_NUM_OF_THREADS]; //array of threads
 for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
 // Launch threads
 t[i] = std::thread(run, std::ref(theBC), i, std::ref(LAC));
 }

 // Wait for threads to finish
 for (int i = 0; i< MAX_NUM_OF_THREADS; i++) {
 t[i].join();
 }

 cout << "All threads terminated" << endl;

//print out the number of times each sensor was accessed
 cout<< "The temperature sensor was accessed "<< counter_ts<<" times."<< endl;
 cout<< "The pressure sensor was accessed "<< counter_ps<<" times."<< endl;
 cout<< "The capacitive sensor was accessed "<< counter_cs<<" times."<< endl;

 //print out all the data in the Receiver
 theReceiver.printSensorData();

 return 0;
}
