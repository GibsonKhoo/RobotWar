#include <iostream> 
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std; 

ofstream output("result.txt"); //storing output in result.txt file

void printOutput(const string& result) //for displaying output in terminal and save in txt file at the same time
{
  cout << result << "\n";
  output << result << "\n";
}

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
    string displayMap;
    for (int j = 0; j < cols; j++)
    {
      displayMap += string(1, table[i][j]) + " "; // add table and blank space to displayMap
    }
    printOutput(displayMap);
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
    int live = 0;

    int * shells;
    int * shellsUsed; 

    int * kill;
    int * lives;

    int * lookUpgradeType; // check which one the robot upgraded to 
    int * moveUpgradeType; 
    int * shootUpgradeType; 

    bool * jump;
    int * remainingJump;

    bool * hide; 
    int * remainingHide; 


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
    virtual void set_shells(int robotIndex);

    void respawnRobot(char targetName);

    void set_kill();

    void set_lives(const string& filename);

    int get_live(int robotIndex){
      return lives[robotIndex];
    }; 

    int count_alive_robots();
    
    void upg_hidebot(int targetIndex);

    void set_initialJump (int robotIndex);

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

      setRobot_num(robot_num); // update the robot number// size
    }
  }

  infile.close(); // close the file

  robotPosX = new int [size]();
  robotPosY = new int [size]();
  shells = new int[size](); 
  shellsUsed = new int[size](); 
  kill = new int[size](); // kill count for each robot
  lives = new int[size](); // lives for each robot

  lookUpgradeType = new int [size](); // 0 = no upgrade, 1 = scout
  for (int i = 0; i < size; i++)
  {
    lookUpgradeType[i] = 0; // default all robot no upgrade 
  }

  moveUpgradeType = new int [size] (); // 0 = no upgrade, 1 = hide, 2 = jump
  for (int i = 0; i < size; i++)
  {
    moveUpgradeType[i] = 0; 
  }

  shootUpgradeType = new int [size] ();
  for (int i = 0; i < size; i++)
  {
    shootUpgradeType[i] = 0; 
  }

  hide = new bool[size](); 
  remainingHide = new int[size](); 

  jump = new bool [size](); 
  remainingJump = new int [size](); 

}

GenericRobot :: ~GenericRobot () // destructor
{
  delete [] robotPosX; 
  delete [] robotPosY; 
  delete [] shells; 
  delete [] shellsUsed; 
  delete [] kill;

  delete [] lookUpgradeType;
  delete [] moveUpgradeType;
  delete [] shootUpgradeType;

  delete [] hide;
  delete [] remainingHide;
  delete [] jump; 
  delete [] remainingJump;
 
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
        printOutput("Error: Robot position not found");
      }
    }
  }
  infile.close();


}

void GenericRobot :: display_robotPos() // display the robot position in the map
{
  // Clear all non-wall cells
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++) {
            if (table[i][j] != '+') {
                table[i][j] = '.';
            }
        }
    }

    // Place alive robots
    for (int i = 0; i < size; i++)
    { 
        if (lives[i] > 0)
        {
            char robot_name = 'A' + i;
            table[robotPosX[i]][robotPosY[i]] = robot_name;
        }
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
      } while (
        table[newX][newY] != '.' || // not empty
        newX == 0 || newX == rows - 1 || newY == 0 || newY == cols - 1 // is border
        );

      // Update robot position
      robotPosX[targetIndex] = newX;
      robotPosY[targetIndex] = newY;
      display_robotPos();

}

void GenericRobot :: set_shells() // set the shells and used shells for each robot
{
  for (int i =0; i < size; i++)
  {
    shells[i] = 10; // each robot has 10 shells
  }

  for (int i = 0; i < size; i++)
  {
    shellsUsed[i] = 0; // each robot has 0 shells used
  }
}

void GenericRobot::set_shells(int robotIndex)
{
  for (int robotIndex; robotIndex < size; robotIndex++)
  {
    shells[robotIndex] = 10; // each robot has 10 shells
  }

}

void GenericRobot :: set_kill() // set the kill count for each robot
{
  for (int i = 0; i < size; i++)
  {
    kill[i] = 0; // each robot has 0 kills
  }
}

void GenericRobot ::set_lives(const string& filename) // set the lives for each robot
{
  fstream infile; 
  infile.open(filename); 
  while (getline (infile,line))
  {
    if (line.find("lives:") != string::npos) // check if the line contain the lives
    {
      int live_start = line.find(":") + 1; 
      int live_num = stoi(line.substr(live_start)); // get the lives number
      live = live_num; // update the lives number
    } 
  }
  infile.close(); 


  for (int i = 0; i < size; i++) // set all robot lives
  {
    lives[i] = live; 
  }

}

int GenericRobot :: count_alive_robots()
{
  int alive = 0;
  for (int i = 0; i < size; ++i) 
  {
      if (lives[i] > 0) 
      {
        alive++;
      }
  }
  return alive;
}

void GenericRobot :: upg_hidebot(int robotIndex) // upgrade the hidebot
{
  hide[robotIndex] = true;
  remainingHide[robotIndex] = 3; 
}

void GenericRobot::set_initialJump (int robotIndex)
{
  remainingJump[robotIndex] = 3;
}




class ShootingRobot : virtual public GenericRobot 
{
public:  
  virtual void shoot(int robotIndex, int dx, int dy, char targetName, bool isUpgraded = false) = 0; // pure virtual function for shooting
};

class MovingRobot : virtual public GenericRobot
{
public:
  virtual void move(const string& filename, int robotIndex, bool isUpgraded = false) = 0; // pure virtual function for moving
};

class ThinkingRobot : virtual public GenericRobot
{
public:
  virtual void think(const string& filename, int robotIndex) = 0; 
};

class LookingRobot : virtual public GenericRobot
{
public:
  virtual void look(int robotIndex, bool isUpgraded = false) = 0; // pure virtual function for looking around
};

class ScoutRobot : public LookingRobot
{
private:
  int remainingScout; // number of scouts available

public: 
  ScoutRobot(const string& filename): GenericRobot(filename), remainingScout(3) {} // default constructor

  void look(int robotIndex, bool isUpgraded = true) override
  {   
    if (remainingScout <= 0)
    {
      return;
    }

    else
    {
      printOutput("ScoutRobot is using scout to scan the battlefield!");
      remainingScout--;

      for (int i = 0; i < getRows(); i++)
      {
        for (int j = 0; j < getCols(); j++)
        {
          char enemy = table[i][j];

          if (enemy >= 'A' && enemy <= 'Z' && enemy != ('A' + robotIndex))
          {
            printOutput("~ Enemy is at (" + to_string(i) + ", " + to_string(j) + ")");
          }
        }
      }
      return; // scout found the enemy
    }
  }
};

class TrackerRobot : public LookingRobot 
{
private:
  struct Tracker // store robot's position
  {
    char name;
  };
  int remainingTracker;
  vector<Tracker> trackedEnemy;
  
public:
  TrackerRobot(const string& filename) : GenericRobot(filename), remainingTracker(3) {} //constructor
  
  void look(int robotIndex, bool isUpgraded = true) override
  {
    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      return;
    }

    if (remainingTracker <= 0)
    {
      printOutput("No tracker left.");
      return;
    }

    else
    {
      
      int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
      int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

      for ( int i = 0; i < 8 && remainingTracker > 0; i++)
      {
        int x = robotPosX[robotIndex] + dx[i];
        int y = robotPosY[robotIndex] + dy[i];

        if (x >= 0 && x < getRows() && y >= 0 && y < getCols())
        {
          char enemy = table[x][y];

          if (enemy >= 'A' && enemy <= 'Z' && enemy != ('A' + robotIndex)) //look for robot other than yourself
          {
            bool enemyTracked = false;
            for (const auto& tracked : trackedEnemy)
            {
              if (tracked.name == enemy)
              {
                enemyTracked = true;
                break;
              }
            }

            if (!enemyTracked)
            {
              trackedEnemy.push_back({enemy});
              remainingTracker--;
              
              printOutput("Tracker planted on robot " + string(1, enemy) + ".");
            }
          }
          else 
          {
            break;
          }
        }
      }
    }
    showTrackedEnemy();
  }

  void showTrackedEnemy() const
    {
      if (trackedEnemy.empty())
      {
        printOutput("TrackerRobot is using tracker, but surrounding have no enemy");
        return;
      }

      printOutput("Tracked enemies: ");
      for (const auto& enemy : trackedEnemy)
      {
        bool found = false;
        for (int i = 0; i < rows; i++)
        {
          for (int j = 0; j < cols; j++)
          {
            if (table[i][j] == enemy.name) // find the tracked robot position on the battlefield
            {
              printOutput("~ Robot " + string(1, enemy.name) + " is now at (" + to_string(i) + ", " + to_string(j) + ")");
              found = true;
            }
          }
        }

        if (!found)
        {
          printOutput("~ Robot " + string(1, enemy.name) + " is not on the battlefield anymore.");
        }

      }
    }
};

class LongShotRobot : public LookingRobot, public ShootingRobot
{
  public:
  LongShotRobot(const string& filename): GenericRobot(filename){} // default constructor

  void look (int robotIndex, bool isUpgraded = true) override
  {

    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      return ;
    }

    else 
    {
      
      for (int dx = -3; dx <= 3; ++dx) // make a 7x7 array, scan area increase 
      {
        for (int dy = -3; dy <= 3; ++dy) 
        {
          if (abs(dx) + abs(dy) > 3 || !(dx == 0 && dy == 0)) // whithin the range but not myself
          {
            
              int target_x = robotPosX[robotIndex] + dx; // target robot position x
              int target_y = robotPosY[robotIndex] + dy;  // target robot position y

              if (target_x >= 0 && target_x < rows && target_y >= 0 && target_y < cols) 
              {
                char target_name = table[target_x][target_y];

                if (target_name >= 'A' && target_name <= 'Z')
                {
                  if (target_name != ('A' + robotIndex)) // check if the target is not itself
                  {
                    shoot(robotIndex, dx, dy, target_name); // fire at the target robot
                    return;

                  }
                }
              }
              
            
            return; // if no target found
          }
        }
      }
    }
  }
};

class ThirtyShotsRobot : public ShootingRobot
{
  public: 
  ThirtyShotsRobot(const string& filename): GenericRobot(filename){} // default constructor

  void set_shells(int robotIndex) override
  {
      shells[robotIndex] = 30; // Give 30 shells to this robot
      printOutput("~ Robot " + string(1, char('A' + robotIndex)) + " now has 30 shells!");
  } 
};

class SemiAutoRobot : public LookingRobot, public ShootingRobot
{
  public: 
  SemiAutoRobot(const string& filename): GenericRobot(filename){} // default constructor
  
  void look (int robotIndex, bool isUpgraded) override
  {

    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      return ;
    }

    else 
    {
      int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1}; //left, up-left, up, down-right, right, down, down-left, up-right
      int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

      if (robotIndex >= size) //check if the robot exist
      {
        printOutput("Invalid robot index.");
        return ;
      }

      else 
      {

        for (int d = 0; d < 8; d++) // check all 8 directions
        {
          int target_x = robotPosX[robotIndex] + dx[d]; // target robot position x
          int target_y = robotPosY[robotIndex] + dy[d];  // target robot position y

          if (target_x >= 0 && target_x < rows && target_y >= 0 && target_y < cols) 
          {
            char target_name = table[target_x][target_y];

            if (target_name >= 'A' && target_name <= 'Z' && target_name != ('A' + robotIndex))
            {
                
                int shots = isUpgraded ? 3 : 1; // 3 shots if upgraded(true), 1 otherwise (false)
                for (int i =0; i <size; i++)
                {
                  shoot(robotIndex, dx[d], dy[d], target_name, true); // fire at the target robot
                  printOutput("\n");
                  
                }
                return;

            }
          }
          else
          {
            printOutput("Out of battlefield at (" + to_string(target_x) + ", " + to_string(target_y) + ")");
          }
        }
        return; // if no target found
      }
    }
  }
};

class HideRobot : public LookingRobot, public ShootingRobot
{
private: 
  int remainHide;

public: 
  HideRobot(const string& filename): GenericRobot(filename), remainHide(3){} // default constructor

};

class JumpRobot : public MovingRobot
{

  public:
    JumpRobot(const string& filename): GenericRobot(filename){} // default constructor

    void move(const string& filename, int robotIndex, bool isUpgraded) override 
    {
      set_initialJump(robotIndex); // set jump to 3

      remainingJump[robotIndex]--;
      printOutput("Remaining Jump upgrade " + to_string(remainingJump[robotIndex]));
      
      // Remove from current position
      table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.';

      // get new pos
      int newX = robotPosX[robotIndex];
      int newY = robotPosY[robotIndex];

      // Find a random empty spot (not on border)
      do {
          newX = rand() % (rows);
          newY = rand() % (cols) ;
      } while (table[newX][newY] != '.');

      if (newX > 0 && newX < getRows() - 1 && newY > 0 && newY < getCols() - 1) // check inside map
      {
        if (newX == robotPosX[robotIndex] && newY == robotPosY[robotIndex]) // check if the robot is not moving
        {
          printOutput("Robot " + string(1, char('A' + robotIndex)) + " stays at (" + to_string(robotPosX[robotIndex]) 
                      + ", " + to_string(robotPosY[robotIndex]) + ")");
        }
        else 
        {
          if (table[newX][newY] != '.') // check if the new position is not empty
          {
            printOutput("Robot " + string(1, char('A' + robotIndex)) + " cannot jump to (" 
                        + to_string(newX) + ", " + to_string(newY) + ") as it is occupied.");
            return; // cannot move to occupied position
          }
          else 
          {
            printOutput("Robot " + string(1, char('A' + robotIndex)) + " jump from (" 
                        + to_string(robotPosX[robotIndex]) + ", " + to_string(robotPosY[robotIndex]) 
                        + ") to (" + to_string(newX) + ", " + to_string(newY) + ")");
            robotPosX[robotIndex] = newX;
            robotPosY[robotIndex] = newY;
          }
          
        }
      }
      
      for (int i = 0; i < getNum_robot(); ++i) 
      {
          table[robotPosX[i]][robotPosY[i]] = char ('A' + i);
      }
      display_robotPos();
      display_map();
      printOutput("\n");
        
    }
  };

class Robot : public ThinkingRobot, 
              public ScoutRobot, 
              public TrackerRobot, 
              public LongShotRobot, 
              public ThirtyShotsRobot, 
              public SemiAutoRobot,
              public HideRobot,
              public JumpRobot// multiple inheritance  
{
public:
  Robot(const string& filename); // constructor

  void shoot(int robotIndex, int dx, int dy, char targetName, bool isUpgraded = false) override 
  {
    if (kill[robotIndex] >= 3 && shootUpgradeType[robotIndex] == 0) 
    {
      int type = rand() % 3; // random upgrade //0 , 1 , 2
      bool taken[4] = {false, false, false, false}; // 0 unused, 1=LongShot, 2=SemiAuto, 3=ThirtyShot

      // Mark which upgrades are taken
      for (int i = 0; i < size; i++) {
          if (i != robotIndex && shootUpgradeType[i] == 1) 
          {
            taken[1] = true;
          }
          if (i != robotIndex && shootUpgradeType[i] == 2) 
          {
            taken[2] = true;
          }
          if (i != robotIndex && shootUpgradeType[i] == 3) 
          {
            taken[3] = true;
          }
      }

      // Try the random upgrade first, then the next, then the last
      // 123, 231, 321
      int tryOrder[3] = {type + 1, (type + 1) % 3 + 1, (type + 2) % 3 + 1}; 
      bool upgraded = false;
      for (int i = 0; i < 3; ++i) {
          int upg = tryOrder[i];
          if (!taken[upg]) {
              shootUpgradeType[robotIndex] = upg;
              string name = (upg == 1) ? "LongShotBot" : (upg == 2) ? "SemiAutoBot" : "ThirtyShotBot";
              
              printOutput("~ Robot " + string(1, char('A' + robotIndex)) + " upgraded to " + name);

              
              upgraded = true;
              break;
          }
      }
      if (!upgraded) {
          printOutput("Shoot upgrades already taken by other robots. No upgrade applied.");
        }
    }

    if (isUpgraded)
    {
      if (shootUpgradeType[robotIndex] == 1) //long shot
      {

        LongShotRobot:: look(robotIndex);  
        
      }
      else if (shootUpgradeType[robotIndex] == 2) // semiauto
      {
        SemiAutoRobot::look(robotIndex, true);
        
      }
      else if (shootUpgradeType[robotIndex] == 3) // thirtyshot
      {
        ThirtyShotsRobot::set_shells(robotIndex);
        
      }
    }
    

    int currentX = robotPosX[robotIndex]; // get the current position of the robot
    int currentY = robotPosY[robotIndex]; // get the current position of the robot

    int targetX = currentX + dx;
    int targetY = currentY + dy;
    int targerNameIndex = targetName - 'A'; // get the index of the target robot
    int hitChance = 100; //random probabiblity to shoot 


    if (dx == 0 && dy == 0) // cannot suicide 
    {
        printOutput("Robot " + string(1, char('A' + robotIndex)) + " tried to fire at itself! Not. allowed.");
        return;
    }

    if (targetX < 0 || targetX >= rows || targetY < 0 || targetY >= cols) // check if surrounding got robot onot
    {
      printOutput("Target out of bounds. Fire action aborted.");
    }
    else 
    {
      if (hitChance > 70)
      {
        printOutput("Robot " + string(1, char('A' + robotIndex)) + "fires at (" + to_string(targetX) 
                    + ", " + to_string(targetY) + ") and hits " + to_string(targetName) + "!");

        //check if the targetrobot is upgraded to hide onot 
        if (hide[targerNameIndex] && remainingHide[targerNameIndex] > 0) 
        {
            printOutput("Robot " + to_string(targetName) + " dodged the attack using HideBot!");
            remainingHide[targerNameIndex]--;
            
            if (remainingHide[targerNameIndex] == 0) 
            {
              printOutput("Hide upgrade is aborted.");
              hide[targerNameIndex] = false; // Hide effect wears off after dodging
            }
            else 
            {
              printOutput("Robot " + to_string(targetName) + " left " + to_string(remainingHide[targerNameIndex]) + " hide.");
            }
            return; // Attack is dodged, do not apply damage
        }

        //normal shooting function 
        if (table[targetX][targetY] != '.' && table[targetX][targetY] != '+')
        {
            respawnRobot(targetName); // shoot and target respawn 
            
        }
        kill[robotIndex]++; 
        lives[targerNameIndex]--;
      }
      else
      {
          printOutput("Robot " + string(1, char('A' + robotIndex)) + " fires at (" + to_string(targetX) + ", " + to_string(targetY) + ") and misses the shot to " + to_string(targetName));
      }

      shells[robotIndex]--;
      shellsUsed[robotIndex]++;

      printOutput("Bullet used: " + to_string(shellsUsed[robotIndex]) + " , Bullets left: " + to_string(shells[robotIndex]));
    }

    for (int i =0; i < size; i++) // for all robots, check if the robot has shells left or lvies 
    {
        if (shells[i] <= 0)
        {
            self_destruct(i); // self destruct the robot
            return; 
        }  
        
        if (lives[i] < 0)
        {
          self_destruct(i);
          return;
        }
    }
  }

  void think(const string& filename, int robotIndex) override 
  {
    bool fired = false;  
    bool found = false; // check if the robot found any target


    look(robotIndex);
    move(filename, robotIndex); 

  } 

  void move(const string& filename, int robotIndex, bool isUpgraded = false) override 
  {  

    if (kill[robotIndex] >= 2 && moveUpgradeType[robotIndex] == 0) {
      int type = rand() % 2; // 0 = Hide, 1 = Jump
      bool taken[3] = {false, false, false}; // 0 unused, 1=Hide, 2=Jump

      // Mark which upgrades are taken
      for (int i = 0; i < size; i++) 
      {
          if (i != robotIndex && moveUpgradeType[i] == 1) 
          {
            taken[1] = true;
          } 
          if (i != robotIndex && moveUpgradeType[i] == 2) 
          {
            taken[2] = true;
          }
      }

      // Try random upgrade first, then the other if needed
      int tryOrder[2] = {type + 1, 3 - (type + 1)};
      bool upgraded = false;
      for (int i = 0; i < 2; ++i)
      {
          int upg = tryOrder[i];
          if (!taken[upg])
           {
              moveUpgradeType[robotIndex] = upg;
              printOutput("~ Robot " + string(1, char('A' + robotIndex)) + " upgraded to " + (upg == 1 ? "HideBot" : "JumpBot"));
              
              upgraded = true;
              break;
          }
      }
      if (!upgraded) {
          printOutput("Move upgrade already taken by another robot. No upgrade applied.");
      }
    }


    if (moveUpgradeType[robotIndex] == 1 && !hide[robotIndex]) // hide // only upgrade when its not upgrade to hide 
    { 
      upg_hidebot(robotIndex); // upgrade to hide        
    }
    else if (moveUpgradeType[robotIndex] == 2) // jump
    {
      if (remainingJump == 0)
      {
        return;
      }
      else
      {
        if ((rand() % 2) == 0)
        {
          JumpRobot :: move(filename, robotIndex, true);
          return; 
        }
        
      }
    }

    // Remove from current position
    table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.';

    int dir = rand() % 8; // Randomly choose a direction

    // get new pos
    int newX = robotPosX[robotIndex];
    int newY = robotPosY[robotIndex];

    switch (dir) 
    {
        case 0: newX -= 1; break; // up
        case 1: newX += 1; break; // down
        case 2: newY -= 1; break; // left
        case 3: newY += 1; break; // right
        case 4 : newX -= 1; newY -= 1; break; // up left
        case 5 : newX -= 1; newY += 1; break; // up right
        case 6 : newX += 1; newY -= 1; break; // down left
        case 7 : newX += 1; newY += 1; break; // down right
        default: break; // stay in place
    }
    

    if (newX > 0 && newX < getRows() - 1 && newY > 0 && newY < getCols() - 1) // check inside map
    {
        if (newX == robotPosX[robotIndex] && newY == robotPosY[robotIndex]) // check if the robot is not moving
        {
          printOutput("Robot " + string(1, char('A' + robotIndex)) + " stays at (" + to_string(robotPosX[robotIndex]) 
                      + ", " + to_string(robotPosY[robotIndex]) + ")");
          
        }
        else 
        {
            if (table[newX][newY] != '.') // check if the new position is not empty
            {
              printOutput("Robot " + string(1, char('A' + robotIndex)) + " cannot move to (" 
                          + to_string(newX) + ", " + to_string(newY) + ") as it is occupied.");
              
              // stays at the pos 
              int newX = robotPosX[robotIndex];
              int newY = robotPosY[robotIndex];
              
            }
            else 
            {
              printOutput("Robot " + string(1, char('A' + robotIndex)) + " moves from (" + to_string(robotPosX[robotIndex]) 
                          + ", " + to_string(robotPosY[robotIndex]) + ") to (" + to_string(newX) + ", " + to_string(newY) + ")");
              

              robotPosX[robotIndex] = newX;
              robotPosY[robotIndex] = newY;
            }
        }
    }

    else 
    {
      printOutput("Robot " + string(1, char('A' + robotIndex)) + " stays at (" + to_string(robotPosX[robotIndex]) 
                    + ", " + to_string(robotPosY[robotIndex]) + ")");
    }

    // display all robots
    for (int i = 0; i < getNum_robot(); ++i) 
    {
        table[robotPosX[i]][robotPosY[i]] = char ('A' + i);
    }
    display_robotPos();
    display_map();
    printOutput("\n");
    
    
  }
  
  void look(int robotIndex, bool isUpgraded = false) override
  {
    if (kill[robotIndex] >= 1 && lookUpgradeType[robotIndex] == 0) 
    {
        int type = rand() % 2; // 0 = Scout, 1 = Track
        bool taken[3] = {false, false, false}; // 0 unused, 1=Scout, 2=Track

        // Mark which upgrades are taken
        for (int i = 0; i < size; i++) 
        {
            if (i != robotIndex && lookUpgradeType[i] == 1) 
            {
              taken[1] = true;
            }
            
            if (i != robotIndex && lookUpgradeType[i] == 2) 
            {
              taken[2] = true;
            }
        }

        // Try random upgrade first, then the other if needed
        // 12, 21
        int tryOrder[2] = {type + 1, 3 - (type + 1)};
        bool upgraded = false;
        for (int i = 0; i < 2; ++i) 
        {
            int upg = tryOrder[i];
            if (!taken[upg]) 
            {
                lookUpgradeType[robotIndex] = upg;
                printOutput("~ Robot " + string(1, char('A' + robotIndex)) + " upgraded to " + (upg == 1 ? "HideBot" : "JumpBot"));
                
                upgraded = true;
                break;
            }
        }
        if (!upgraded) 
        {
            printOutput("Look upgrades already taken by other robots. No upgrade applied.");
        }
    
    }
      
    
    if (lookUpgradeType[robotIndex] == 1)
    {
      if (rand() % 2 == 0) // 50% to use the skills
      { 
        ScoutRobot::look(robotIndex);
      }
    }
    else if (lookUpgradeType[robotIndex] == 2)
    {
      TrackerRobot :: look(robotIndex);
    }

    // Up, Up-Right, Right, Down-Right, Down, Down-Left, Left, Up-Left
    int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1}; 
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    
    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      return ;
    }

      
      for (int d = 0; d < 8; d++) // check all 8 directions
      {
        int target_x = robotPosX[robotIndex] + dx[d]; // target robot position x
        int target_y = robotPosY[robotIndex] + dy[d];  // target robot position y

        if (target_x >= 0 && target_x < rows && target_y >= 0 && target_y < cols) 
        {
          char target_name = table[target_x][target_y];

          if (target_name >= 'A' && target_name <= 'Z' && target_name != ('A' + robotIndex))
          {
            shoot(robotIndex, dx[d], dy[d], target_name); // fire at the target robot
            return;       
          }
        }
        else
        {
          printOutput("Out of battlefield at (" + to_string(target_x) + ", " + to_string(target_y) + ")");
        }
      }
      return; // if no target found
    }
  
};

Robot ::Robot(const string& filename) : GenericRobot(filename), 
                                        ScoutRobot(filename), 
                                        TrackerRobot(filename), 
                                        LongShotRobot(filename),
                                        ThirtyShotsRobot(filename), 
                                        SemiAutoRobot(filename),
                                        HideRobot(filename),
                                        JumpRobot (filename) // constructor
{
  srand((unsigned)time(0)); // make sure the random number generator is seeded
                            // so that the robot can move randomly
} 


int main()
{
  string filename = "game.txt"; // file name
  Robot robot(filename); // Only one Robot object

  int numRobots = robot.getNum_robot();
  robot.set_steps(filename);
  int numSteps = robot.get_steps();
  robot.GenericRobot::set_shells(); // preset all shells = 10
  robot.set_lives(filename); // preset all lives = 3

  robot.create_map(); 
  robot.get_robotPos(filename);
  robot.display_robotPos(); // display the robot position in the map
  robot.display_map();


  bool gameStart = true;
  int round = 1;
  
  while (gameStart && numSteps > 0) // game starts and steps are available
  {
    printOutput("---------------- Round " + to_string(round) + "-----------------");
    
    for (int robotIndex = 0; robotIndex < numRobots; ++robotIndex) // each robot takes turn
    {
      robot.display_robotPos(); // update robot positions on the map

      if (robot.get_live(robotIndex) > 0) // robot alive
      {
        printOutput("Lives robot " + string(1, char('A' + robotIndex)) + ": " + to_string(robot.get_live(robotIndex)));
        robot.think(filename, robotIndex); //4 function execute
      }

      else  
      {
        robot.self_destruct(robotIndex);
        printOutput("Robot " + string(1, char('A' + robotIndex)) + " is dead. \n");
        

        if (robot.count_alive_robots() == 1 ) // if last robot, end game 
        {
          printOutput("Game Over");
          
          gameStart = false;
          return 0;
        }
      }
    }
    numSteps--; 
    round++; 

    if (numSteps <= 0) 
    {
      printOutput("Game Over");
      
      gameStart = false; // end the game
    }
  }

  robot.destroy_map(); // deallocate the memory for the robot map

  output.close();
  return 0; 
}