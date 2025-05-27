#include <iostream> 
#include <fstream>
#include <string>

using namespace std; 

class Map 
{
  protected: 
    char** table; // put 2d array pointer in private so can be access from public 
    int rows; 
    int cols; 
    string line; 
 

  public: 
    void setMap (int r , int c) {
      rows = r; 
      cols = c; 
    }
    int getRows (){
      return rows;
    }

    int getCols () {
      return cols;
    }
    Map(const string& filename); // get the map size // constructor
    
    virtual ~Map ();// destructor

    void create_map(); 
  
    void destroy_map();// memory deallocation

    void display_map () const;      

};

Map ::Map(const string& filename) // get the map size // constructor
{
  ifstream infile; 

  infile.open(filename); // open the file
  while (getline (infile,line))
  {
    if (line.find("Rows") != string::npos) // check if the line contain the rows
    {
      int r_start = line.find(":") + 1; 
      int r = stoi(line.substr(r_start, line.find(" ")));  // get the rows
      int c_start = line.find(",") + 1; 
      int c = stoi(line.substr(c_start)); // get the cols
    
      setMap(r,c); // set the map size
    }
  }
  infile.close(); // close the file
}

Map :: ~Map () // destructor
{
  cout << "Destroying map..." << endl;
  destroy_map(); // deallocate the memory
}

void Map :: create_map() // create the map
{
  table = new char*[rows];
  for (int i = 0; i < rows; i++)
  {
      table[i] = new char[cols]();
      for (int j = 0; j < cols; j++) // assign all element to '.'
      {
        if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) 
        {
          table [i][j] = '+'; 
        }
        else
        {
          table [i][j] = '.';
        }
      }
  }  
}

void Map :: destroy_map() // deallocate the memory
{
  for (int i = 0; i < rows; i++)
  {
    delete [] table[i]; // deallocate the memory
  }
  delete [] table; // deallocate the memory
}

void Map :: display_map() const // display the map
{
  for (int i =0; i < rows; i++)
  { 
    for (int j = 0; j < cols; j++)
    {
      cout << table [i][j] << " "; 
    }
    cout << endl;
  }
}




class GenericRobot : public Map // initial class for robot
{
  protected:  
    int * robotPosX; // robot position x
    int * robotPosY; // robot position y

    int size = 0; //will be updated to 5
    int currentIndex = 0;

    int steps = 0; //will be updated to 300

  public:
    void setPos_array (int s) {
      size = s; 
    }

    int getPos_array () const {
      return size; 
    }

    GenericRobot(const string& filename); // constructor header 
  
    virtual ~GenericRobot (); // destructor header
    
    void get_robotPos(const string& filename);

    void display_robotPos();// display the robot position in the map

    void set_steps(const string& filename);

    int get_steps() const {
      return steps; // return the steps number
    }
  };

GenericRobot :: GenericRobot(const string& filename) : Map (filename) // constructor
{
  fstream infile; 
  infile.open(filename); 
  while (getline (infile,line))
  {
    if (line.find("robots") != string :: npos)
    {
      int robot_start = line.find(":") + 1;
      int robot_num = stoi (line.substr(robot_start)); // get the robot number

      //cout << "Robot number: " << robot_num << endl;
      setPos_array(robot_num); // update the robot number// size
    }
  }

  infile.close(); // close the file 

  robotPosX = new int [size]();
  robotPosY = new int [size]();
}

GenericRobot :: ~GenericRobot () // destructor
{
  delete [] robotPosX; // deallocate the memory
  delete [] robotPosY; // deallocate the memory 
}

void GenericRobot ::get_robotPos(const string& filename) // get the robot position
{
  fstream infile; 
  infile.open(filename); 
  while (getline (infile,line))
  {
    if (line.find("GenericRobot") != string::npos) 
    {
      if (line.find("Pos") != string::npos) // check if the line contain the robot position
      {
        int x_start = line.find("(") + 1; 
        int x = stoi(line.substr(x_start, line.find(",")));  // get the robot position x
        int first_comma = line.find(","); 
        int y_start = line.find(",", first_comma + 1) + 1; // get the second comma in the line
        int y = stoi(line.substr(y_start, line.find(")"))); // get the robot position y

        if (x >= 0 && x < rows && y >= 0 && y < cols) // check if the robot position is in the map
        {  
          robotPosX[currentIndex] = x; 
          robotPosY[currentIndex] = y; 
          currentIndex++; 
        }
      }

      else // random 
      {
        cout << "Error: Robot position not found" << endl;
      }
    }
  }
  infile.close();


}

void GenericRobot :: display_robotPos() // display the robot position in the map
{
      for (int i = 0; i < size; i++)
      { 
        char robot_name = 'A'+ i; // assign the robot position to 'R'
        table[robotPosX[i]][robotPosY[i]] = robot_name; // assign the robot position to 'R'
      }
}

void GenericRobot :: set_steps(const string& filename) // get the steps
{
  fstream infile; 
  infile.open(filename); 
  while (getline (infile,line))
  {
    if (line.find("steps:") != string::npos) // check if the line contain the steps
    {
      int steps_start = line.find(":") + 1; 
      int steps_num = stoi(line.substr(steps_start)); // get the steps number
      steps = steps_num; // update the steps number
      
    } 
  }
  infile.close(); // close the file 
}




class ShootingRobot // class for shoot function
{
public:
  virtual void shoot() = 0; // pure virtual function for shooting
};

class MovingRobot // class for move function
{
public:
  virtual void move() = 0; // pure virtual function for moving
};

class ThinkingRobot // class for think function
{
public:
  virtual void think() = 0; // pure virtual function for thinking
};

class LookingRobot // class for look function
{
public:
  virtual void look(int robotIndex, int offsetX, int offsetY) = 0; // pure virtual function for looking around
};

class ScoutRobot // class for scout funtion
{
public:
  virtual void scout(int robotIndex, int offsetX, int offsetY) = 0; // pure virtual function for scout function
};


class Robot : public GenericRobot, public ShootingRobot, public MovingRobot, public ThinkingRobot, public LookingRobot // multiple inheritance  
{
public:
  Robot(const string& filename); // constructor

  void shoot() override 
  {
    cout << "Robot shoots!" << endl; 
  }
  void move() override 
  {
    cout << "Robot moves!" << endl; 
  }
  void think() override 
  {
    cout << "Robot is thinking!" << endl; 
  } 

  void look(int robotIndex, int offsetX, int offsetY) override
  {
    if (robotIndex >= size) //check if the robot exist
    {
      cout << "Invalid robot index." << endl;
      return;
    }

    if (offsetX < -1 || offsetX > 1 || offsetY < -1 || offsetY > 1) //to make sure robot is only looking one step around it
    {
      cout << "Error: you can only look at 8 neighbouring location or your own location." << endl;
    }

    int centreX = robotPosX[robotIndex] + offsetX;
    int centreY = robotPosY[robotIndex] + offsetY;

    cout << "Robot " << char('A' + robotIndex) << " looks around at area (" << centreX << ", " << centreY << ")" << endl;

    for (int dx = -1; dx <= 1; dx++) //left to right
    {
      for (int dy = -1; dy <= 1; dy++) //top to bottom
     {
       int newX = centreX + dx;
       int newY = centreY + dy;

        if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) //to check if robot is in battlefield range
        {
          char cell = table[newX][newY];
        
          if (newX == robotPosX[robotIndex] && newY == robotPosY[robotIndex])
          {
           cout << "Own self is at (" << newX << ", " << newY << ")." << endl;
          }

          else if (cell >= 'A' && cell <= 'Z') //robot found
          {
            cout << "Enemy is at (" << newX << ", " << newY << ")" << endl;
          }

          else
          {
           cout << "(" << newX << ", " << newY << ") is empty." << endl;
          }
        }

        else
        {
         cout << "Out of battlefield at (" << newX << ", " << newY << ")" << endl;
        }
      }
    }
  }
};

class ScoutRobot : public Robot
{
private:
  int remainingScout;

public:
  ScoutRobot(const string& filename) : Robot(filename), remainingScout(3) {}

  void look(int robotIndex, int offsetX, int offsetY) override
  {
    if (remainingScout <= 0)
    {
      cout << "No scouts left." << endl;
      return;
    }

    else
    {
      cout << "ScoutRobot is using scout to scan the battlefield!" << endl;
      remainingScout--;

      for (int i = 0; i < getRows(); i++)
      {
        for (int j = 0; j < getCols(); j++)
        {
          char cell = table[i][j];

          if (cell >= 'A' && cell <= 'Z')
          {
            cout << "Enemy is at (" << i << ", " << j << ")" << endl;
          }
        }
      }
    }
  }
};

Robot ::Robot(const string& filename) : GenericRobot(filename){} // constructor


int main()
{
  string filename = "game.txt"; // file name
  Robot tempRobot(filename); // get the robot num from the generic robot class
  int numRobots = tempRobot.getPos_array(); // get the robot size

  tempRobot.set_steps(filename); // get the steps
  int numSteps = tempRobot.get_steps(); // get the steps number
  cout << "Number of Steps: " << numSteps << endl; // display the number of robots


  Robot* robots [numRobots]; // create an array of robot pointers
  for (int i = 0; i < numRobots; i++)
  {
    robots[i] = new Robot(filename); // create a robot object for each robot
  }


  //initialize the map for each robot
  for (int i = 0; i < numRobots; i++)
  {
    robots[i]->create_map();
    robots[i]->get_robotPos(filename); // get the robot position
    robots[i]->display_robotPos();
  }  
    robots[0]->display_map(); // display the map for the first robot

  for (int i = 0; i < numRobots; ++i) 
  {
    cout << "\n--- Robot " << char('A' + i) << " actions ---\n";
    robots[i]->think();
    robots[i]->move();
    robots[i]->shoot();
    robots[i]->look(i, 0, 0);
  }
  
  for (int i = 0; i < numRobots; ++i) 
  {
        delete robots[i];
        robots[i]->destroy_map(); // deallocate the memory for each robot
  }

  return 0; 
}