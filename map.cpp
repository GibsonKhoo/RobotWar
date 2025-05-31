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
  cout << "Game Over" << endl;
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
      // cout << table [i][j] << " "; 
    }
    printOutput(displayMap);
    // cout << endl;
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

    bool * lookUpgraded; // check if robot is upgraded for looking 
    int * lookUpgradeType; // check which one the robot upgraded to 

    

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

    void set_kill();

    void set_lives(const string& filename);

    int get_live(int robotIndex){
      return lives[robotIndex];
    }; 

    int check_map();
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
  kill = new int[size](); // kill count for each robot
  lives = new int[size](); // lives for each robot

  lookUpgraded = new bool [size](); // check if each robot upgraded for looking 
  lookUpgradeType = new int [size](); // 0 = no upgrade, 1 = scout
  for (int i = 0; i < size; i++)
  {
    lookUpgradeType[i] = 0; // default all robot no upgrade 
  }




}

GenericRobot :: ~GenericRobot () // destructor
{
  delete [] robotPosX; 
  delete [] robotPosY; 
  delete [] shells; 
  delete [] shellsUsed; 
  delete [] kill;

  delete [] lookUpgraded;
  delete [] lookUpgradeType;
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
        // cout << "Error: Robot position not found" << endl;
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

      printOutput("Robot " + string(1, char('A' + targetIndex)) + " respawned at (" + to_string(newX) + ", " + to_string(newY) + ")");
      // cout << "Robot " << char ('A' + targetIndex) << " respawned at (" << newX << "," << newY << ")" << endl;
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

int GenericRobot :: check_map()
{
  int robot_alive = 0;

  for (int i = 0; i < getRows(); i++)
  {
    for (int j = 0; j < getCols(); j++ )  
    {
      if (table[i][j] != '.' && table[i][j] != '+')
      {
        robot_alive ++;
      }
    }
    
  }
  return robot_alive;
}


class ShootingRobot : virtual public GenericRobot 
{
public:  
  virtual void shoot(int robotIndex, int dx, int dy, char targetName) = 0; // pure virtual function for shooting
};

class MovingRobot : virtual public GenericRobot
{
public:
  virtual void move(const string& filename, int robotIndex) = 0; // pure virtual function for moving
};

class ThinkingRobot : virtual public GenericRobot
{
public:
  virtual void think(const string& filename, int robotIndex) = 0; 
};

class LookingRobot : virtual public GenericRobot
{
public:
  virtual void look(int robotIndex) = 0; // pure virtual function for looking around
};

class ScoutRobot : public LookingRobot
{
private:
  int remainingScout; // number of scouts available

public: 
  ScoutRobot(const string& filename): GenericRobot(filename), remainingScout(3) {} // default constructor

  void look(int robotIndex) override
  {   
    if (remainingScout <= 0)
    {
      // cout << "No scouts left." << endl;
      return;
    }

    else
    {
      printOutput("ScoutRobot is using scout to scan the battlefield!");
      // cout << "Scout " << char('A'+ robotIndex) << " is using scout to scan the battlefield!" << endl;
      remainingScout--;

      for (int i = 0; i < getRows(); i++)
      {
        for (int j = 0; j < getCols(); j++)
        {
          char enemy = table[i][j];

          if (enemy >= 'A' && enemy <= 'Z')
          {
            printOutput("Enemy is at (" + to_string(i) + ", " + to_string(j) + ")");
            // cout << "Enemies is at (" << i << ", " << j << ")" << endl; // display all the enemy pos
          }
        }
      }
      return; // scout found the enemy
    }
  }
};

struct Tracker // store robot's position
{
  char name;
};

class TrackerRobot : public LookingRobot 
{
private:
  int remainingTracker = 3;
  vector<Tracker> trackedEnemy;

public:
  TrackerRobot(const string& filename) : GenericRobot(filename) {}
  void look(int robotIndex) override
  {
    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      // cout << "Invalid robot index." << endl;
      return;
    }

    if (remainingTracker <= 0)
    {
      printOutput("No tracker left.");
      // cout << "No tracker left." << endl;
      return;
    }

    else
    {
      printOutput("TrackerRobot is using tracker to track the enemies!");
      // cout << "TrackerRobot is using tracker to track the enemies!" << endl;

      int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
      int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

      for ( int i = 0; i < 8 && remainingTracker > 0; i++)
      {
        int x = robotPosX[robotIndex] + dx[i];
        int y = robotPosY[robotIndex] + dy[i];

        if (x >= 0 && x < getRows() && y >= 0 && y < getCols())
        {
          char cell = table[x][y];

          if (cell >= 'A' && cell <= 'Z' && cell != ('A' + robotIndex)) //look for robot other than yourself
          {
            bool enemyTracked = false;
            for (const auto& tracked : trackedEnemy)
            {
              if (tracked.name == cell)
              {
                enemyTracked = true;
                break;
              }
            }

            if (!enemyTracked)
            {
              trackedEnemy.push_back({cell});
              remainingTracker--;

              cout << "Tracker planted on robot " << cell << "." << endl;
            }
          }
        }
      }

      return //!trackedEnemy.empty();

    }
  }

  void showTrackedEnemy() const
    {
      if (trackedEnemy.empty())
      {
        printOutput("No robots tracked.");
        // cout << "No robots tracked." << endl;
        return;
      }

      printOutput("Tracked enemies: ");
      // cout << "Tracked enemies:" << endl;
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
              // cout << "~ Robot " << enemy.name << " is now at (" << i << ", " << j << ")" << endl;
              found = true;
            }
          }
        }

        if (!found)
        {
          printOutput("~ Robot " + string(1, enemy.name) + " is not on the battlefield anymore.");
          // cout << "~ Robot " << enemy.name << " is not on the battlefield anymore." << endl;
        }

      }
    }
};

class Robot : public ShootingRobot, public MovingRobot, public ThinkingRobot, public ScoutRobot// multiple inheritance  
{
public:
  Robot(const string& filename); // constructor

  void shoot(int robotIndex, int dx, int dy, char targetName) override 
  {
    int currentX = robotPosX[robotIndex]; // get the current position of the robot
    int currentY = robotPosY[robotIndex]; // get the current position of the robot

    int targetX = currentX + dx;
    int targetY = currentY + dy;
    int targerNameIndex = targetName - 'A'; // get the index of the target robot
    int hitChance = rand() % 100; //random probabiblity to shoot 


    if (dx == 0 && dy == 0) // cannot suicide 
    {
        printOutput("Robot " + string(1, char('A' + robotIndex)) + " tried to fire at itself! Not allowed.");
        // cout << "Robot " << char('A' + robotIndex) << " tried to fire at itself! Not allowed." << endl;
        return;
    }

    if (targetX < 0 || targetX >= rows || targetY < 0 || targetY >= cols) // check if surrounding got robot onot
    {
        printOutput("Target out of bounds. Fire action aborted.");
        // cout << "Target out of bounds. Fire action aborted." << endl;
    }
    else 
    {
      if (hitChance < 70)
      {
          printOutput("Robot " + string(1, char('A' + robotIndex)) + " fires at (" + to_string(targetX) + ", " + to_string(targetY) 
                      + ") and hits " + string(1, targetName) + "!");
          // cout << "Robot " << char('A' + robotIndex) << " fires at (" << targetX << "," << targetY << ") and hits " << targetName << "!" << endl;
          if (table[targetX][targetY] != '.' && table[targetX][targetY] != '+')
          {
              respawnRobot(targetName); // shoot and target respawn 
          }
          kill[robotIndex]++; 
          lives[targerNameIndex]--;
      }
      else
      {
        printOutput("Robot " + string(1, char('A' + robotIndex)) + " fires at (" + to_string(targetX) + ", " + to_string(targetY) + ") and misses.");
        // cout << "Robot " << char('A' + robotIndex) << " fires at (" << targetX << "," << targetY << ") and misses." << endl;
      }

      shells[robotIndex]--;
      shellsUsed[robotIndex]++;

      printOutput("Bullets used: " + to_string(shellsUsed[robotIndex]) + ", Bullets left: " + to_string(shells[robotIndex]));
      // cout << "Bullets used: " << shellsUsed[robotIndex] << ", Bullets left: " << shells[robotIndex] << endl; 
    }

    for (int i =0; i < size; i++) // for all robots, check if the robot has shells left or lvies 
    {
        if (shells[i] <= 0)
        {
            printOutput("Robot " + string(1, char('A' + i)) + " has no shells left and self-destructs!");
            // cout << "Robot " << char('A' + i) << " has no shells left and self-destructs!" << endl;
            self_destruct(i); // self destruct the robot
            return; 
        }  
        
        if (lives[i] < 0)
        {
          cout << "Robot " << char('A' + i) << "has no live" << endl;
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

  void move(const string& filename, int robotIndex) override 
  {  
    // Remove from current position
    table[robotPosX[robotIndex]][robotPosY[robotIndex]] = '.';

    int dir = rand() % 8; // Randomly choose a direction

    // get new pos
    int newX = robotPosX[robotIndex];
    int newY = robotPosY[robotIndex];

    switch (dir) {
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
        // cout << "Robot '" << char ('A' + robotIndex) << "' stays at (" 
              // << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ")" << endl;
      }
      else 
      {
        if (table[newX][newY] != '.') // check if the new position is not empty
        {
          cout << "Robot '" << char ('A' + robotIndex) << "' cannot move to (" 
                << newX << "," << newY << ") as it is occupied." << endl;
          
          // stays at the pos 
          int newX = robotPosX[robotIndex];
          int newY = robotPosY[robotIndex];
          return;
        }
        else 
        {
          printOutput("Robot " + string(1, char('A' + robotIndex)) + " moves from (" + to_string(robotPosX[robotIndex]) 
                      + ", " + to_string(robotPosY[robotIndex]) + ") to (" + to_string(newX) + ", " + to_string(newY) + ")");
          // cout << "Robot '" << char ('A' + robotIndex) << "' moves from (" 
          //      << robotPosX[robotIndex] << "," << robotPosY[robotIndex] << ") to (" 
          //      << newX << "," << newY << ")" << endl;
          robotPosX[robotIndex] = newX;
          robotPosY[robotIndex] = newY;
        }
        
      }
      
    }

    else 
    {
      cout << "Robot '" << char ('A' + robotIndex) << "' cannot move outside the battlefield." << endl;
    }

    // display all robots
    for (int i = 0; i < getNum_robot(); ++i) 
    {
        table[robotPosX[i]][robotPosY[i]] = char ('A' + i);
    }
    display_map();
    printOutput("\n");
    // cout << endl;
       
  }
  
  void look(int robotIndex) override
  {
      if (kill[robotIndex] == 1 && lookUpgradeType[robotIndex] == 0) // if the robot has killed any robot and no upgrade
      {
        int type = rand() % 2; // random upgrade to scout or track //0 , 1
        bool upgradeTaken = false;
        

        for (int i =0; i < size; i++) // Check if any other robot already has this upgrade
        {
          if (i != robotIndex && lookUpgradeType[i] == type + 1) // type+1: 1=Scout, 2=Track
          {
            upgradeTaken = true;
            break;
          }
        }
        
        if (!upgradeTaken) 
        {
        switch (type)
        {
          case 0: 
              lookUpgradeType[robotIndex] = 1;
              cout << "upgraded to ScoutBot" << endl;
              break;

          case 1: 
              lookUpgradeType[robotIndex] = 2;  
              cout << "upgraded to TrackBot" << endl; 
              break;

        }
        lookUpgraded[robotIndex] = true;
        
      }
      else 
      {
        cout << "Upgrade already taken by another robot. No upgrade applied." << endl;
      }
    }

    if (lookUpgradeType[robotIndex] == 1)
    {
      if (rand() % 2 == 0) // 50% to use the skills
      { 
            ScoutRobot::look(robotIndex);
            // return; // either can use skills next round after upgrade, or instant can use  
        }
    }
    else if (lookUpgradeType[robotIndex] == 2)
    {
      cout << "TrackBot function" << endl;
    }


    int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1}; //left, up-left, up, down-right, right, down, down-left, up-right
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    
    if (robotIndex >= size) //check if the robot exist
    {
      printOutput("Invalid robot index.");
      // cout << "Invalid robot index." << endl;
      return ;
    }

    else 
    {
      printOutput("Robot " + string(1, char('A' + robotIndex)) + " looks around at area (" + to_string(robotPosX[robotIndex]) + ", " + to_string(robotPosY[robotIndex]) + ")");
      // cout << "Robot " << char('A' + robotIndex) << " looks around at area (" << robotPosX[robotIndex] << ", " << robotPosY[robotIndex]  << ")" << endl;

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
              printOutput("Robot A detects robot " + string(1, target_name) + " at (" + to_string(target_x) + ", " + to_string(target_y) + ")");
              // cout << "Robot A detects robot " << target_name << " at (" << target_x << "," << target_y << ")" << endl;
              shoot(robotIndex, dx[d], dy[d], target_name); // fire at the target robot
              return;

            }
          }
        }
        else
        {
          printOutput("Out of battlefield at (" + to_string(target_x) + ", " + to_string(target_y) + ")");
          // cout << "Out of battlefield at (" << target_x << ", " << target_y << ")" << endl;
        }
      }
      return; // if no target found
    }
  }

};

Robot ::Robot(const string& filename) : GenericRobot(filename), ScoutRobot(filename) // constructor
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
  robot.set_shells(); 
  robot.set_lives(filename); // preset all lives = 3

  bool gameStart = true;
  int round = 1;
  
  while (gameStart && numSteps > 0) // game starts and steps are available
  {
    printOutput("---------------- Round " + to_string(round) + "-----------------");
    // cout << " ---------------- Round " << round  << " -----------------" << endl;
    for (int robotIndex = 0; robotIndex < numRobots; ++robotIndex) // each robot takes turn
    {

      cout << "Lives robot " << char('A' + robotIndex) << " : "<< robot.get_live(robotIndex) << endl; // display each robot lives every round
      robot.think(filename, robotIndex); //4 function execute

      if (robot.check_map() == 1) // if last robot, end game 
      {
        cout << "Game Over" << endl;
        gameStart = false;
      }
    }

    numSteps--; 
    round++; 

    if (numSteps <= 0) 
    {
      gameStart = false; // end the game
    }
  }

  robot.destroy_map(); // deallocate the memory for the robot map

  output.close();
  return 0; 
}