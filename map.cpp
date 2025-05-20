#include <iostream> 
#include <fstream>
#include <string>
using namespace std; 

class File
{
  protected: 
    string filename; // file name
    ifstream infile; 
    ofstream outfile; 
    string line; 

  public: 
    void read_file (const string& filename)
    {
      infile.open(filename); // open the file
      if (infile.fail())
      {
        cout << "Error opening file: " << filename << endl; // check if the file is open
        return;
      }
      else 
      {

        while (getline(infile, line)) // read the file line by line
        {
          cout << line << endl; // print out the line
        }
      }

      infile.close(); // close the file
    }
        
    void write_file (const string& filename)
    {
      outfile.open(filename); // open the file
      if (outfile.fail())
      {
        cout << "Error opening file: " << filename << endl; // check if the file is open
        return;
      }
      
      outfile.close(); // close the file
    }


};



class Map
{
  protected: 
    char** table; // put 2d array pointer in private so can be access from public 
    int rows; 
    int cols; 
    string line; 
 

  public: 

    void setMap (int r , int c) // set the map size
    {
      rows = r; 
      cols = c; 
    }
    int getRows ()
    {
      return rows;
    }

    int getCols () 
    {
      return cols;
    }

    Map(const string& filename) // get the map size // constructor
    { 
      ifstream infile; // create a file stream

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

    void create_map() 
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

    // virtual ~Map () // destructor
    // {
    //   // cout << "Map destructor called" << endl;
    //   for (int i =0; i < rows; i++)
    //   {
    //     delete [] table[i]; 
    //   }
    //   delete [] table;
    // }
    

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


/*
class GenericRobot : public Map
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
*/


int main()
{
  File file;
  string filename = "game.txt"; // file name
  Map map(filename); 
  
  
  cout << "Rows: " << map.getRows() << ", Cols: " << map.getCols() << endl;
  map.create_map();
  map.display_map();



  return 0; 
}