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

    int size = 0; //will be updated to 5
    int currentIndex = 0;

    int steps = 0; //will be updated to 300

    int * shells;
    int * shellsUsed; 

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

    void self_destruct(int robotIndex);

    void set_shells (); 
    void respawnRobot(char targetName);
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
  shells = new int[size](); 
  shellsUsed = new int[size](); 
}

GenericRobot :: ~GenericRobot () // destructor
{
  delete [] robotPosX; 
  delete [] robotPosY; 
  delete [] shells; 
  delete [] shellsUsed; 
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


void GenericRobot :: self_destruct(int robotIndex) // self destruct the robot
{
  table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.'; 
}

void GenericRobot :: respawnRobot (char targetName)
{
      int targetIndex = targetName - 'A'; // flw ascii value to get the index
      int newX, newY;

      // Remove from old position
      self_destruct(targetIndex);

      // Find a random empty spot
      do {
          newX = rand() % rows; 
          newY = rand() % cols;
      } while (table[newX][newY] != '.');

      // Update robot position
      robotPosX[targetIndex] = newX;
      robotPosY[targetIndex] = newY;

      cout << "Robot " << char ('A' + targetIndex) << " respawned at (" << newX << "," << newY << ")" << endl;
}

void GenericRobot :: set_shells() // set the shells and used shells for each robot
{
  for (int i = 0; i < size; i++)
  {
    shells[i] = 10; // each robot has 10 shells
  }

  for (int i = 0; i < size; i++)
  {
    shellsUsed[i] = 0; // each robot has 0 shells used
  }
}

class ShootingRobot
{
public:  
  virtual void shoot(int robotIndex, int dx, int dy, char targetName) = 0; // pure virtual function for shooting
};

class MovingRobot 
{
public:
  virtual void move(const string& filename, int robotIndex) = 0; // pure virtual function for moving
};

class ThinkingRobot 
{
public:
  virtual void think(const string& filename, int robotIndex) = 0; 
};

class LookingRobot 
{
public:
  virtual bool look(int robotIndex) = 0; // pure virtual function for looking around
};

class ScoutRobot : public LookingRobot, public GenericRobot
{
private:
  int remainingScout; // number of scouts available

public: 
  ScoutRobot(const string& filename): GenericRobot(filename), remainingScout(3) {} // constructor

  bool look(int robotIndex) override
  {
    if (remainingScout <= 0)
    {
      cout << "No scouts left." << endl;
      return false;
    }

    else
    {
      cout << "ScoutRobot is using scout to scan the battlefield!" << endl;
      remainingScout--;

      for (int i = 0; i < getRows(); i++)
      {
        for (int j = 0; j < getCols(); j++)
        {
          char enemy = table[i][j];

          if (enemy >= 'A' && enemy <= 'Z')
          {
            cout << "Enemy is at (" << i << ", " << j << ")" << endl; // display all the enemy pos
          }
        }
      }
      return true; // scout found the enemy
    }
  }
};


class Robot : public GenericRobot, public ShootingRobot, public MovingRobot, public ThinkingRobot, public LookingRobot// multiple inheritance  
{
public:
  Robot(const string& filename); // constructor

  void shoot(int robotIndex, int dx, int dy, char targetName) override 
  {
    int currentX = robotPosX[robotIndex]; // get the current position of the robot
    int currentY = robotPosY[robotIndex]; // get the current position of the robot

    int targetX = currentX + dx;
    int targetY = currentY + dy;

    int hitChance = rand() % 100; //random probabiblity to shoot 


    if (dx == 0 && dy == 0) // cannot suicide 
    {
        cout << "Robot " << char('A' + robotIndex) << " tried to fire at itself! Not allowed." << endl;
        return;
    }

    if (targetX < 0 || targetX >= rows || targetY < 0 || targetY >= cols) // check if surrounding got robot onot
    {
        cout << "Target out of bounds. Fire action aborted." << endl;
    }
    else 
    {
      if (hitChance < 70)
      {
          cout << "Robot " << char('A' + robotIndex) << " fires at (" << targetX << "," << targetY << ") and hits " << targetName << "!" << endl;
          if (table[targetX][targetY] != '.' && table[targetX][targetY] != '+')
          {
              respawnRobot(targetName); // shoot and target respawn 
          }
      }
      else
      {
          cout << "Robot " << char('A' + robotIndex) << " fires at (" << targetX << "," << targetY << ") and misses." << endl;
      }

      shells[robotIndex]--;
      shellsUsed[robotIndex]++;

      cout << "Bullets used: " << shellsUsed[robotIndex] << ", Bullets left: " << shells[robotIndex] << endl; 
    }

    for (int i =0; i < size; i++) // for all robots, check if the robot has shells left
    {
        if (shells[i] <= 0)
        {
            cout << "Robot " << char('A' + i) << " has no shells left and self-destructs!" << endl;
            self_destruct(i); // self destruct the robot
            return; 
        }   
    }
  }

  void think(const string& filename, int robotIndex) override 
  {
    bool fired = false;  
    bool found = false; // check if the robot found any target

    if (look(robotIndex) == true)
    {
      found = true; 
      fired = true; 
      move(filename, robotIndex); 
    }

    else
    {
      cout << "Robot " << char ('A' + robotIndex) <<" found no nearby targets to fire at." << endl;
      move(filename, robotIndex); 
    }


  } 

  void move(const string& filename, int robotIndex) override 
  {  
    // Remove from current position
    table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.';

    int dir = rand() % 8; // Randomly choose a direction

    // get new pos
    int newX = robotPosX[robotIndex];
    int newY = robotPosY[robotIndex];

    switch (dir) {
        case 1: newX -= 1; break; // up
        case 2: newX += 1; break; // down
        case 3: newY -= 1; break; // left
        case 4: newY += 1; break; // right
        case 5 : newX -= 1; newY -= 1; break; // up left
        case 6 : newX -= 1; newY += 1; break; // up right
        case 7 : newX += 1; newY -= 1; break; // down left
        case 8 : newX += 1; newY += 1; break; // down right
        default: break; // stay in place
    }

    if (newX > 0 && newX < getRows() - 1 && newY > 0 && newY < getCols() - 1) // check inside map
    {
      if (newX == robotPosX[robotIndex] && newY == robotPosY[robotIndex]) // check if the robot is not moving
      {
        cout << "Robot '" << char ('A' + robotIndex) << "' stays at (" 
              << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ")" << endl;
      }
      else 
      {
        if (table[newX][newY] != '.') // check if the new position is not empty
        {
          cout << "Robot '" << char ('A' + robotIndex) << "' cannot move to (" 
                << newX << "," << newY << ") as it is occupied." << endl;
          return; // cannot move to occupied position
        }
        else 
        {
          cout << "Robot '" << char ('A' + robotIndex) << "' moves from (" 
                << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ") to (" 
                << newX << "," << newY << ")" << endl;
          robotPosX[robotIndex] = newX;
          robotPosY[robotIndex] = newY;
        }
        
      }
      
    }

    // display all robots
    for (int i = 0; i < getNum_robot(); ++i) 
    {
        table[robotPosX[i]][robotPosY[i]] = char ('A' + i);
    }
    display_map();
    cout << endl;
       
  }
  
  bool look(int robotIndex) override
  {
    int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1}; //left, up-left, up, down-right, right, down, down-left, up-right
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};


    if (robotIndex >= size) //check if the robot exist
    {
      cout << "Invalid robot index." << endl;
      return false;
    }

    else 
    {
      cout << "Robot " << char('A' + robotIndex) << " looks around at area (" << robotPosX[robotIndex] << ", " << robotPosY[robotIndex]  << ")" << endl;

      for (int d = 0; d < 8; d++) // check all 8 directions
      {
        int target_x = robotPosX[robotIndex] + dx[d]; // target robot position x
        int target_y = robotPosY[robotIndex] + dy[d];  // target robot position y

        if (target_x >= 0 && target_x < rows && target_y >= 0 && target_y < cols) 
        {
          char target_name = table[target_x][target_y];

          if (target_name >= 'A' && target_name <= 'Z')
          {
            if (target_name != ('A' + robotIndex)) // check if the target is not itself
            {
              cout << "Robot A detects robot " << target_name << " at (" << target_x << "," << target_y << ")" << endl;
              shoot(robotIndex, dx[d], dy[d], target_name); // fire at the target robot
              return true;

            }
          }

          // checking if the looking function is working correctly 
          // if (target_x == robotPosX[robotIndex] && target_y == robotPosY[robotIndex])
          // {
          //   cout << "Own self is at (" << target_x << ", " << target_y << ")." << endl;
          // }

          // else if (target >= 'A' && target <= 'Z') //robot found
          // {
          //   cout << "Enemy is at (" << target_x << ", " << target_y << ")" << endl;
          // }

          // else
          // {
          //   cout << "(" << target_x << ", " << target_y << ") is empty." << endl;
          // }
        }
        else
        {
          cout << "Out of battlefield at (" << target_x << ", " << target_y << ")" << endl;
        }
      }
      return false; // if no target found
    }
  }

};

Robot ::Robot(const string& filename) : GenericRobot(filename) // constructor
{
  srand((unsigned)time(0)); // make sure the random number generator is seeded
                            // so that the robot can move randomly
} 



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
  robot.set_shells(); // set the shells for each robot

  bool gameStart = true;
  int round = 1;
  
  while (gameStart && numSteps > 0) // game starts and steps are available
  {
    
    cout << " ---------------- Round " << round  << " -----------------" << endl;
    for (int robotIndex = 0; robotIndex < numRobots; ++robotIndex) // each robot takes turn
    {
        robot.think(filename, robotIndex); 
    }

    numSteps--; 
    round++; 

    if (numSteps <= 0) 
    {
      gameStart = false; // end the game
    }
  }

  robot.destroy_map(); // deallocate the memory for the robot map
  return 0; 
}