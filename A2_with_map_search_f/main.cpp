// Flavia Dumitrica


// Pre-processor directives
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;

// Initializing pseudo-random number generator
std::mt19937 gen(time(0));
std::uniform_int_distribution<> dis(1, 1000); // Uniformly distributed random numbers between 1 and 1000

// Global constants
int const MAX_NUM_OF_THREADS = 6; // Maximum number of threads allowed in program
int const NUM_OF_LINKS = 2; // Number of communication links
int const NUM_OF_SAMPLES = 50; // Number of data samples
string t_s = "temperature_sensor";
string p_s = "pressure_sensor";
string c_s = "capacitive_sensor";

// Global variables to keep track of the number of times each sensor was accessed
int counter_ts = 0;
int counter_ps = 0;
int counter_cs = 0;

// Map for thread Ids
std::unordered_map<std::thread::id, int> threadIDs; // Stores the thread IDs and their associated integers

// Function which atomizes the printing action
void printing(std::string str) {
  static std::mutex m; // A static mutex for synchronizing print statements
  std::lock_guard<std::mutex> lg(m);  // Locks the mutex during the printing operation
  cout << str << endl;
}

// Search the map to find the associated integer for a particular thread ID
int search() {
  std::unordered_map<std::thread::id, int>::iterator it =
      threadIDs.find(std::this_thread::get_id());
  if (it == threadIDs.end())
    return -1;  // Thread 'id' NOT found
  else
    return it->second;  // Thread 'id' found, return the associated integer
}

// Utility class to store sensor data
class SensorData {
 public:
  // Constructor
  SensorData(string type) : sensor_type(type) {}

  string getSensorType() { return sensor_type; }
  std::deque<double> getSensorData() { return sensor_data; }
  void addData(double newData) { sensor_data.push_back(newData); }

 private:
  string sensor_type = "";
  std::deque<double> sensor_data; // Deque to store the sensor data
};

// Class Receiver: Receives and stores sensor data for each sensor type
class Receiver {
 public:
  static Receiver& GetInstance() {
    static Receiver s_instance;
    return s_instance;
  }

  // Receives a SensorData object and stores the data in corresponding containers:
  void receiveData(SensorData sd) {
    std::unique_lock<std::mutex> Rec_ul(mtx); // Locks the mutex to avoid race condition
    std::deque<double> sensdata = sd.getSensorData(); // Retrieve sensor data
    if (sd.getSensorType() == t_s) {
      temp_data.insert(temp_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == p_s) {
      press_data.insert(press_data.end(), sensdata.begin(), sensdata.end());
    }
    if (sd.getSensorType() == c_s) {
      cap_data.insert(cap_data.end(), sensdata.begin(), sensdata.end());
    }
  }

  // Prints out all data for each sensor
  void printSensorData() {
    cout << "temperature data:" << endl;
    for (deque<double>::iterator it = temp_data.begin(); it != temp_data.end();
         ++it) {
      cout << *it << endl;
    }

    cout << "pressure data:" << endl;
    for (deque<double>::iterator it = press_data.begin();
         it != press_data.end(); ++it) {
      cout << *it << endl;
    }

    cout << "capacitance data:" << endl;
    for (deque<double>::iterator it = cap_data.begin(); it != cap_data.end();
         ++it) {
      cout << *it << endl;
    }
  }

 private:
  std::mutex mtx; // Mutex to synchronize access to data containers
  std::deque<double> temp_data;
  std::deque<double> press_data;
  std::deque<double> cap_data;

  // Constructor (private to ensure singleton behavior)
  Receiver() {}

};

// Class Link: Represents a communication link between sensors and the receiver
class LinkAccessController; // Forward declaration

class Link {
 public:
  // Constructor
  Link(Receiver& r, int linkNum)
      : inUse(false), myReceiver(r), linkId(linkNum), observer(nullptr) {}

  void attachObserver(LinkAccessController* lac); // Attach a LinkAccessController object as an observer
  void notifyObserver(); // Notify the observer about the link status change

  // Check if the link is currently in use
  bool isInUse() { return inUse; }

  // Set the link status to busy
  void setInUse() { inUse = true; }

  // Set the link status to idle
  void setIdle() {
    inUse = false;
    notifyObserver(); // Notify the observer that the link is idle now
  }
  // Write data to the receiver
  void writeToDataLink(SensorData sd) { myReceiver.receiveData(sd); }

  // Returns the link Id
  int getLinkId() { return linkId; }

 private:
  bool inUse; // Flag to indicate if the link is currently in use
  Receiver& myReceiver; // Receiver reference to store sensor data
  int linkId;
  LinkAccessController* observer; // Observer (LinkAccessController) to notify when the link becomes idle
}; 

// Class LinkAccessController: Controls access to communication links
class LinkAccessController {
 public:
  static LinkAccessController& GetInstance() {
    static LinkAccessController lac_instance(Receiver::GetInstance());
    return lac_instance;
  }
  // Request a comm's link: returns a reference to an available Link.
  // If none are available, the calling thread is suspended.
  Link& requestLink() {
    int linkNum;
    std::unique_lock<std::mutex> LAC_ul(LAC_mu);
    while (commsLinks[0].isInUse() && commsLinks[1].isInUse()) {
      stringstream s6;
      s6 << "Thread " << search() << " is suspended on links" << endl;
      printing(s6.str());
      LAC_cv.wait(LAC_ul); // Wait until one of the links becomes idle
    }
    Link* requestedLink = nullptr;
    for (Link& elem : commsLinks) {
      // Find the first available link
      if (!elem.isInUse()) {
        requestedLink = &elem;
        requestedLink->setInUse(); // Set the link status to busy
        break;
      }
    }
    return *requestedLink;
  }

  // Release a comm. link:
  void releaseLink(Link& releasedLink) {
    std::unique_lock<std::mutex> LAC_ul(LAC_mu);
    releasedLink.setIdle();
  }

  void update(Link* link) {
    // If the link becomes idle notify one waiting thread
    if (!link->isInUse()) {
      LAC_cv.notify_one();
    }
  }

 private:
  Receiver& myReceiver;  // Receiver reference
  int numOfAvailableLinks;
  std::vector<Link> commsLinks; // Vector to store communication links
  std::mutex LAC_mu; // Mutex to synchronize access to communication links
  std::condition_variable LAC_cv; // Condition variable for thread synchronization

  // Constructor (private to ensure singleton behavior)
  LinkAccessController(Receiver& r)
      : myReceiver(r), numOfAvailableLinks(NUM_OF_LINKS) {
    for (int i = 0; i < NUM_OF_LINKS; i++) {
      commsLinks.push_back(Link(myReceiver, i)); // Create communication links and attach observer
      commsLinks[i].attachObserver(this);
    }
  }

};

void Link::attachObserver(LinkAccessController* lac) { observer = lac; } // Attach a LinkAccessController as an observer

void Link::notifyObserver() {
  if (observer) {
    observer->update(this); // Notify the observer (LinkAccessController) about the link status change
  }
}

// Abstract base class that models a sensor
class Sensor {
 public:
  // Constructor
  Sensor(string& type) : sensorType(type) {}

  // Destructor
  virtual ~Sensor() {}

  // Declare a pure virtual method to be overridden by derived classes
  virtual double getValue() = 0;

  // Declare non-virtual method:
  string getType() {
    // Returns the type of Sensor that this is:
    return sensorType;
  }

  // Declare any instance variable(s):
  string sensorType = "";

};

// Derived class for temperature sensor
class TempSensor : public Sensor {
 public:
  // Constructor
  TempSensor(string& s) : Sensor(s) {}

  // Return a random value of ambient temperature between 10 and 30
  virtual double getValue() { return (dis(gen) % 21 + 10.0); } 

};

// Derived class for pressure sensor
class PressureSensor : public Sensor {
 public:
  // Constructor
  PressureSensor(string& s) : Sensor(s) {}

  // Return a random value of pressure between 95 and 105
  virtual double getValue() { return (dis(gen) % 11 + 95.0); }  

};  

// Derived class for capacitive sensor
class CapacitiveSensor : public Sensor {
 public:
  // Constructor
  CapacitiveSensor(string& s) : Sensor(s) {}

  // Return a random value of capacitance between 1 and 5
  virtual double getValue() { return (dis(gen) % 5 + 1.0); }  

}; 

enum class SensorType {
  temperature_sensor,
  pressure_sensor,
  capacitive_sensor
};

// Factory function to create instances of sensor objects based on SensorType enum
Sensor* MakeSensorFactory(SensorType type) {
  if (SensorType::temperature_sensor == type) {
    return new TempSensor(t_s);
  } else if (SensorType::pressure_sensor == type) {
    return new PressureSensor(p_s);
  } else if (SensorType::capacitive_sensor == type) {
    return new CapacitiveSensor(c_s);
  }
  return nullptr;
}

// Class BC (Bus Controller): Manages access to sensor data and communication links
class BC {
 public:
  static BC& GetInstance(std::vector<Sensor*>& sensors) {
    static BC bc_instance(sensors);  // Only one instance will be created
    return bc_instance;
  }

  void requestBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    while (lock) {
      std::stringstream s;
      s << "Bus Controller is locked so thread " << search()
        << " is about to suspend..." << endl;
      printing(s.str());
      vc.wait(ul); // Wait until the bus controller becomes available
    }
    lock = true; // Set the BC lock to true (bus controller is locked)
    std::stringstream s1;
    s1 << "Bus Controller locked by thread " << search() << endl;
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
    lock = false; // Set the BC lock to false (bus controller is unlocked)
    vc.notify_one(); // Notify one waiting thread that the bus controller is available now
    std::stringstream s3;
    s3 << "Bus Controller UNlocked by thread "
       << threadIDs[std::this_thread::get_id()] << endl;
    printing(s3.str());
  }

 private:
  // Constructor initialises a vector of Sensor pointers that are passed in by reference:
  BC(std::vector<Sensor*>& sensors) : theSensors(sensors) {}

  bool lock = false; // 'false' means that the BC is not locked (unlocked)
  std::vector<Sensor*>& theSensors; // Reference to a vector of Sensor pointers
  std::mutex BC_mu; // Mutex to synchronize access to the bus controller
  std::condition_variable vc; // Condition variable for thread synchronization

}; 

// Run function executed by each thread
void run(BC& theBC, int idx, LinkAccessController& lac) {
  int sel;
  double val;
  string typ;

  SensorData t_s_data(
      t_s); // Initialise temperature values for this thread to zero
  SensorData p_s_data(
      p_s); // Initialise pressure values for this thread to zero
  SensorData c_s_data(
      c_s); // Initialise capacitance values for this thread to zero

  // Implementing a hash-map to add the ID of the thread safely
  std::mutex map_mu; // Declare a mutex to synchronize access to the threadIDs map
  std::unique_lock<std::mutex> map_locker(map_mu); // Lock the map via the mutex
  threadIDs.insert(std::make_pair(std::this_thread::get_id(), idx)); // Insert the current thread ID and its associated integer into the threadIDs map
  map_locker.unlock(); // Unlock the map

  // Acquire values from the sensors and transmit to receiver
  for (int i = 0; i < NUM_OF_SAMPLES; i++) {
    val = 0;
    typ = "";
    sel = 0;

    theBC.requestBC(); // Request use of the BC:

    // Generate a random value between 0 and 2, and use it to
    // Select a sensor and obtain a value and the sensor's type:
    sel = dis(gen) % 3;
    val = theBC.getSensorValue(sel);
    typ = theBC.getSensorType(sel);

    // Print out sensor type, sample value and thread id
    std::stringstream s2;
    s2 << " sample value from thread " << idx << " of type " << typ
       << " is equal to " << val << endl;
    printing(s2.str());

    // Increment counter for the chosen sensor
    if (typ == t_s) {
      counter_ts++;
      t_s_data.addData(val);
    }

    if (typ == p_s) {
      counter_ps++;
      p_s_data.addData(val);
    }

    if (typ == c_s) {
      counter_cs++;
      c_s_data.addData(val);
    }

    theBC.releaseBC(); // Release the BC:

    // Delay for random period between 1 and 10 milliseconds:
    std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));

  }  

  Link* link = &lac.requestLink(); // Request a communication link
  std::stringstream s4;
  s4 << "Thread " << idx << " successfully acquired link " << link->getLinkId()
     << endl;
  printing(s4.str());

  // Write sensor data to the receiver via the link
  link->writeToDataLink(t_s_data);
  link->writeToDataLink(p_s_data);
  link->writeToDataLink(c_s_data);

  // Release the communication link
  lac.releaseLink(*link);

  std::stringstream s5;
  s5 << "Thread " << idx << " successfully released link " << link->getLinkId()
     << endl;
  printing(s5.str());

  // Delay for random period between 1 and 10 milliseconds
  std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));

}

int main() {
  // Declare a vector of Sensor pointers:
  std::vector<Sensor*> sensors;

  // Initialise each sensor and insert into the vector
  sensors.push_back(MakeSensorFactory(SensorType::temperature_sensor));
  sensors.push_back(MakeSensorFactory(SensorType::pressure_sensor));
  sensors.push_back(MakeSensorFactory(SensorType::capacitive_sensor));

  // Instantiate and start the threads:
  std::thread t[MAX_NUM_OF_THREADS];  // create array of threads
  for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
    t[i] = std::thread(
        run, std::ref(BC::GetInstance(sensors)), i,
        std::ref(LinkAccessController::GetInstance())); // Launch the threads with run function as the entry point
  }

  // Wait for the threads to finish and join
  for (int i = 0; i < MAX_NUM_OF_THREADS; i++) {
    t[i].join();
  }

  cout << "All threads terminated" << endl;

  // Print out the number of times each sensor was accessed
  cout << "The temperature sensor was accessed " << counter_ts << " times."
       << endl;
  cout << "The pressure sensor was accessed " << counter_ps << " times."
       << endl;
  cout << "The capacitive sensor was accessed " << counter_cs << " times."
       << endl;

  // Print out all the data in the Receiver
  Receiver::GetInstance().printSensorData();

  // Deallocate memory
  for (Sensor* sensor : sensors) {
    delete sensor;
  }

  return 0;
} 
