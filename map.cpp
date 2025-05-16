#include <iostream> 
using namespace std; 

class Map
{
  protected: 
    char** table; // put 2d array pointer in private so can be access from public 
    int rows = 20; 
    int cols = 10; 

  public: 
    Map() //constructor 
    {
      table = new char*[rows];
      for (int i = 0; i < rows; i++)
      {
          table[i] = new char[cols];
          for (int j = 0; j < cols; j++) // assign all element to '.'
          {
            table [i][j] = '.';
          }
      }
    }

    virtual ~Map () // destructor
    {
      for (int i =0; i < rows; i++)
      {
        delete [] table[i]; 
      }
      delete [] table;
    }
    

    void display_map () const 
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

};

class GenericRobot : Map
{
  private: 
    int robotPosX;
    int robotPosY;

  public:
    GenericRobot() // constructor // spawn of the generic robot in btw the map
    {
      robotPosX = rand() % rows; 
      robotPosY = rand() % cols;
    }

    void RobowSpawn ()
    {
      cout << "x=" << robotPosX << ", y=" << robotPosY << endl; 
      

    }
};


int main()
{
  Map map; 
  GenericRobot grobot; 
  grobot.RobowSpawn();
  map.display_map();


  return 0; 
}