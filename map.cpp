#include <iostream> 
#include <fstream>
#include <string>
#include <ctime>


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
    char* robotinitials; // robot initials

    int size = 0; //will be updated to 5
    int currentIndex = 0;

    int steps = 0; //will be updated to 300

  public:
    void setRobot_num (int s) {
      size = s; 
    }

    int getNum_robot () const {
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
      setRobot_num(robot_num); // update the robot number// size
    }
  }

  infile.close(); // close the file 

  robotPosX = new int [size]();
  robotPosY = new int [size]();
  robotinitials = new char[size](); // initials array
}

GenericRobot :: ~GenericRobot () // destructor
{
  delete [] robotPosX; // deallocate the memory
  delete [] robotPosY; // deallocate the memory 
  delete [] robotinitials; // deallocate the memory
}

void GenericRobot ::get_robotPos(const string& filename) // get the robot position
{
  fstream infile; 
  infile.open(filename); 
  while (getline (infile,line))
  {
    if (line.find("GenericRobot") != string::npos) 
    {
      size_t name_pos = line.find("Name:");
      char initial = ' ';
      if(name_pos != string::npos){
        size_t name_start = name_pos + 5; // Move past "Name;"
        while (line[name_start] == ' ' || line[name_start] == ':') name_start++;
        initial = line[name_start];
      }

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
          robotinitials[currentIndex] = initial; 
          currentIndex++; 
        }
      }
      else // random 
      {
        cerr << "Error: Robot position not found" << endl;
      }
    }
  }
  infile.close();
}

void GenericRobot :: display_robotPos() // display the robot position in the map
{
  for (int i = 0; i < size; i++)
  { 
    table[robotPosX[i]][robotPosY[i]] = robotinitials[i]; // use the robot's initial
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
  virtual void move(const string& filename, int robotIndex) = 0; // pure virtual function for moving
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

class Upgrade // class for upgrade function
{
public: 
  virtual void look(int robotIndex, int offsetX, int offsetY) = 0; 

  virtual ~Upgrade() {} // virtual destructor for cleanup
};


class Robot : public GenericRobot, public ShootingRobot, public MovingRobot, public ThinkingRobot, public LookingRobot // multiple inheritance  
{
public:
  Robot(const string& filename); // constructor

  void shoot() override 
  {
    cout << "Robot shoots!" << endl; 
  }

  void think() override 
  {
    cout << "Robot is thinking!" << endl; 
  } 

  void move(const string& filename, int robotIndex) override 
  {  
    // Remove from current position
    table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.';

    int dir = rand() % 4; // Randomly choose a direction

    // get new pos
    int newX = robotPosX[robotIndex];
    int newY = robotPosY[robotIndex];
    switch (dir) {
        case 1: newX -= 1; break;
        case 2: newX += 1; break;
        case 3: newY -= 1; break;
        case 4: newY += 1; break;
        case 5 : newX -= 1; newY -= 1; break; // up left
        case 6 : newX -= 1; newY += 1; break; // up right
        case 7 : newX += 1; newY -= 1; break; // down left
        case 8 : newX += 1; newY += 1; break; // down right
        default: break; // stay in place
    }

    if (newX > 0 && newX < getRows() - 1 && newY > 0 && newY < getCols() - 1) 
    {
        cout << "Robot '" << robotinitials[robotIndex] << "' moves from (" 
              << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ") to (" 
              << newX << "," << newY << ")" << endl;
        robotPosX[robotIndex] = newX;
        robotPosY[robotIndex] = newY;
    } 
    else 
    {
        cout << "Robot '" << robotinitials[robotIndex] << "' stays at (" 
              << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ")" << endl;
    }

    // display all robots
    for (int i = 0; i < getNum_robot(); ++i) 
    {
        table[robotPosX[i]][robotPosY[i]] = robotinitials[i]; 
    }
    display_map();
    cout << endl;
       
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

          // else
          // {
          //  cout << "(" << newX << ", " << newY << ") is empty." << endl;
          // }
        }

        else
        {
         cout << "Out of battlefield at (" << newX << ", " << newY << ")" << endl;
        }
      }
    }
  }
};

Robot ::Robot(const string& filename) : GenericRobot(filename) // constructor
{
  srand((unsigned)time(0)); // make sure the random number generator is seeded
                            // so that the robot can move randomly    
} 


class ScoutRobot : public Upgrade, public Robot // ScoutRobot inherits from Upgrade and Robot
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



int main()
{
  string filename = "game.txt"; // file name
  Robot robot(filename); // Only one Robot object

  robot.create_map(); 
  robot.get_robotPos(filename); 
  robot.display_robotPos(); // display the robot position in the map
  robot.display_map(); 

  int numRobots = robot.getNum_robot();
  robot.set_steps(filename);
  int numSteps = robot.get_steps();

  bool gameStart = true;
  int round = 1;
  
  while (gameStart && numSteps > 0) // game starts and steps are available
  {
    
    cout << "Round " << round << endl;
    for (int robotIndex = 0; robotIndex < numRobots; ++robotIndex) 
    {
        
        robot.think();
        robot.shoot();
        robot.look(robotIndex, 0, 0); 
        robot.move(filename, robotIndex); 
    }

    numSteps--; // decrease the steps after each robot move
    round++; // increase the round after each robot move

    if (numSteps <= 0) 
    {
      gameStart = false; // end the game
    }
  }

  robot.destroy_map(); // deallocate the memory for the robot map
  return 0; 
}