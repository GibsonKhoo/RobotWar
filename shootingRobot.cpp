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

        bool fire(char robotName, int currentX, int currentY, int dx, int dy)
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

        //int hitChance = rand() % 100;
        int hitChance = 0;

        if (hitChance < 70)
        {
                cout << "Robot " << robotName << " fires at (" << targetX << "," << targetY << ") and hits!" << endl;
            if (table[targetX][targetY] != '.' && table[targetX][targetY] != '+')
            {
                table[targetX][targetY] = 'X'; // Mark destroyed robot
            }
        }
        else
        {
            cout << "Robot " << robotName << " fires at (" << targetX << "," << targetY << ") and misses." << endl;
        }

        cout << "Bullets used: " << shellsUsed << ", Bullets left: " << shells << endl;
        return true;
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
  robot.fire('A', 2, 6, -1, 2);
  robot.display_map(); // display the map
  cout << "Closing program..." << endl;



  return 0;
}
