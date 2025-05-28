#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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


class Robot : public Map
{
  private:
    int * robotPosX; // robot position x
    int * robotPosY; // robot position y

    int size = 0; //will be updated to 5
    int currentIndex = 0;

  public:
    void setPos_array (int s) {
      size = s;
    }

    int getPos_array () const {
      return size;
    }

    Robot(const string& filename); // constructor

    virtual ~Robot (); // destructor

    void get_robotPos(const string& filename);

    void display_robotPos ();// display the robot position in the map
};

Robot :: Robot(const string& filename) : Map (filename) // constructor
{
  fstream infile;
  infile.open(filename);
  while (getline (infile,line))
  {
    if (line.find("robots") != string :: npos)
    {
      int robot_start = line.find(":") + 1;
      int robot_num = stoi (line.substr(robot_start)); // get the robot number

      cout << "Robot number: " << robot_num << endl;
      setPos_array(robot_num); // update the robot number// size
    }
  }

  infile.close(); // close the file

  robotPosX = new int [size]();
  robotPosY = new int [size]();
}

Robot :: ~Robot () // destructor
{
  delete [] robotPosX; // deallocate the memory
  delete [] robotPosY; // deallocate the memory
}

void Robot ::get_robotPos(const string& filename) // get the robot position
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
        int x = stoi(line.substr(x_start, line.find(",")));  
        int first_comma = line.find(",");
        int y_start = line.find(",", first_comma + 1) + 1; 
        int y = stoi(line.substr(y_start, line.find(")")));


        if (x >= 0 && x < rows && y >= 0 && y < cols) // check if the robot position is in the map
        {
          robotPosX[currentIndex] = x;
          robotPosY[currentIndex] = y;
          cout << "Robot " << char('A' + currentIndex) << " is at (" << x << "," << y << ")" << endl;
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

void Robot :: display_robotPos() // display the robot position in the map
{
      for (int i = 0; i < size; i++)
      {
        if (table[robotPosX[i]][robotPosY[i]] != 'X')
        {
          char robot_name = 'A'+ i; // assign the robot position to 'R'
          table[robotPosX[i]][robotPosY[i]] = robot_name; // assign the robot position to 'R'

        }
      }
}

class Shooting : public Robot
{
private:
    int shells;
    int shellsUsed = 0;

public:
    Shooting(const string& filename, int initialShells = 10)
        : Robot(filename), shells(initialShells), shellsUsed(0)
    {
        srand(time(0)); // Seed random once
    }

    bool fire(char robotName, int currentX, int currentY, int dx, int dy, char targetName)
    {
        if (shells <= 0)
        {
            cout << "Robot " << robotName << " has no shells left and self-destructs!" << endl;
            table[currentX][currentY] = 'X'; // mark self-destruct
            return false;
        }

        if (dx == 0 && dy == 0)
        {
            cout << "Robot " << robotName << " tried to fire at itself! Not allowed." << endl;
            return false;
        }

        int targetX = currentX + dx;
        int targetY = currentY + dy;

        if (targetX < 0 || targetX >= rows || targetY < 0 || targetY >= cols)
        {
            cout << "Target out of bounds. Fire action aborted." << endl;
            return false;
        }

        shells--;
        shellsUsed++;

        int hitChance = 0; // guaranteed hit

        if (hitChance < 70)
        {
            cout << "Robot " << robotName << " fires at (" << targetX << "," << targetY << ") and hits " << targetName << "!" << endl;
            if (table[targetX][targetY] != '.' && table[targetX][targetY] != '+')
            {
                table[targetX][targetY] = 'X'; // Mark destroyed robot
                respawnRobot(targetName, targetX, targetY);
            }
        }
        else
        {
            cout << "Robot " << robotName << " fires at (" << targetX << "," << targetY << ") and misses." << endl;
        }

        cout << "Bullets used: " << shellsUsed << ", Bullets left: " << shells << endl;
        return true;
    }

    // Find coordinates of a robot by name (like 'A')
    bool findRobotPosition(char robotName, int &x, int &y)
    {
        for (int i = 0; i < getRows(); i++)
        {
            for (int j = 0; j < getCols(); j++)
            {
                if (table[i][j] == robotName)
                {
                    x = i;
                    y = j;
                    return true;
                }
            }
        }
        return false;
    }

    void autoFireNearest()
    {
      int ax = -1, ay = -1;
      
      //to find Robot A
      for (int i = 0; i < rows; i++)
      {
        for (int j = 0; j < cols; j++)
        {
          if (table[i][j] == 'A')
          {
            ax = i;
            ay = j;
            break;
          }
        }
        if (ax != -1) break;
      }

      if (ax == -1 || ay == -1)
      {
        cout << "Robot A not found." << endl;
        return;
      }

      int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
      int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

      bool fired = false;

      for (int d = 0; d < 8; d++)
      {
        int nx = ax + dx[d];
        int ny = ay + dy[d];

      if (nx >= 0 && nx < rows && ny >= 0 && ny < cols)
      {
        char target = table[nx][ny];

        if (target >= 'B' && target <= 'Z')
        {
          cout << "Robot A detects robot " << target << " at (" << nx << "," << ny << ")" << endl;
          fire('A', ax, ay, dx[d], dy[d], target);
          fired = true;
          break;
        }
      }
      } 
    

    if (!fired)
    {
      cout << "Robot A found no nearby targets to fire at." << endl;
    }
}

void respawnRobot(char robotName, int oldX, int oldY)
{
  table[oldX][oldY] = '.';

  int attempts = 0;
  const int MAX_ATTEMPTS = 100;

  while (attempts < MAX_ATTEMPTS)
  {
    int x = rand() % rows;
    int y = rand() % cols;

    if (table[x][y] == '.')
    {
      table[x][y] = robotName;
      cout << "Robot " << robotName << " has respawned at (" << x << "," << y << ")" << endl;
      return;
    }

    attempts++;
  }

  cout << "Failed to respawn robot " << robotName << ". No free space found." << endl;
}
int getShells() const
    {
        return shells;
    }
};

    
int main()
{
  string filename = "game.txt"; // file name
  Map map(filename);
  Shooting robot(filename); // create a robot object

  robot.create_map();
  robot.get_robotPos(filename); // get the robot position
  robot.display_robotPos();
  robot.autoFireNearest();
  robot.display_map(); // display the map


  return 0;
};