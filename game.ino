// Array of cells:
int cells[8][8];
// Buffer for state of cells while changing other array:
int cellsBuffer[8][8];
// Flag detects an update that didn't actually change anything:
bool repeated;

int generations; // number of generations elapsed since start of game
int maxGenerations = 100; // maximum number of generations before game is restarted

bool serialDebugging = true; // true or false

void setup() {
  // LED array setup
  pinMode(Max7219_pinCLK,OUTPUT);
  pinMode(Max7219_pinCS,OUTPUT);
  pinMode(Max7219_pinDIN,OUTPUT);
  delay(50);
  Init_MAX7219();
  
  // LIFE setup  
  startGame();
  repeated = false;
  generations = 1;
  
  // serial
  if (serialDebugging) { Serial.begin(9600); }
}
 
void loop() {
  if ( repeated || (generations > maxGenerations)) {
    startGame();
    generations = 1;
  }

  // write cell contents to LED matrix
  showCells();
  
  // update game state
  generation();
 
  // check if current generation is different from previous one
  repeated = repeatedScreen();
  
  // delay between generations
  delay(500);
}

void generation() {
  if (serialDebugging) { Serial.println("Generation "+String(generations)+String(":")); }

  // copy cells to buffer
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      cellsBuffer[x][y] = cells[x][y];
    }
  } 

  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) { 
      // Count live neighbouring cells, with wrap-around
      int xm1 = (x-1+8)%8;  // "x minus 1" -- with wrap-around
      int xp1 = (x+1+8)%8;  // "x plus  1" -- with wrap-around
      int ym1 = (y-1+8)%8;
      int yp1 = (y+1+8)%8;
      int neighbours = cellsBuffer[xm1][yp1] + cellsBuffer[x][yp1] + cellsBuffer[xp1][yp1];
      neighbours    += cellsBuffer[xm1][y]                         + cellsBuffer[xp1][y];
      neighbours    += cellsBuffer[xm1][ym1] + cellsBuffer[x][ym1] + cellsBuffer[xp1][ym1];
      //Serial.println("Cell (x,y)=("+String(x)+","+String(y)+"): brackets ["+String(xm1)+",x,"+String(xp1)+"], ["+String(ym1)+",y,"+String(yp1)+"], Neighbours: "+String(neighbours));
      if (cellsBuffer[x][y] == 1) { 
        // Current cell is alive
        if (neighbours < 2 || neighbours > 3) {
          //Serial.println("Cell (x,y)=("+String(x)+","+String(y)+") dies");
          cells[x][y] = 0;
          }
      } else { 
        // Current cell is dead
        if (neighbours == 3) {
          //Serial.println("Cell (x,y)=("+String(x)+","+String(y)+") is born");
          cells[x][y] = 1;
          }
      } // end of if
    }
  }
  generations++;
}

void startGame() {
  if (serialDebugging) { Serial.println("\nNEW GAME"); }

  // setup
  generations = 1;
  randomSeed(analogRead(0));

  flashScreen();
  
  // initialize each cell
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      float state = random(100);
      state > 35 ? state = 0 : state = 1; // probability of cell being alive at start
      cells[x][y] = state;
    }
  }
  
  if (serialDebugging) {
    // Start a glider
    cells[0] = {0,0,0,0,0,0,0,0};
    cells[1] = {0,0,0,0,0,0,0,0};
    cells[2] = {0,0,0,0,0,1,0,0};
    cells[3] = {0,0,0,0,0,0,1,0};
    cells[4] = {0,0,0,0,1,1,1,0};
    cells[5] = {0,0,0,0,0,0,0,0};
    cells[6] = {0,0,0,0,0,0,0,0};
    cells[7] = {0,0,0,0,0,0,0,0};
  }
}

void flashScreen() {
  int off = B00000000;
  int on = B11111111;
 
  for (int r=1; r<9; r++) { Write_Max7219(r, off); }
  delay(50);
  for (int r=1; r<9; r++) { Write_Max7219(r, on); }
  delay(50);
  for (int r=1; r<9; r++) { Write_Max7219(r, off); }
  delay(50);
}

bool repeatedScreen() {
  for (int x=0; x<8; x++) {
    for (int y=0; y<8; y++) {
      if (cells[x][y] != cellsBuffer[x][y]) {
        return false;
      }
    }
  }
  return true;
}

void showCells() {
  // convert cells array to binary values that LED matrix can read
  String debugLine = String("");
  for (int x=0; x<8; x++) {
    // For column with offset x, build and send a 1-byte pattern
    if (serialDebugging) { debugLine = "x = "+String(x)+": "; }

    byte oneBit  = 1;
    byte thisRow = 0;
    for (int y=0; y<8; y++) {
      if (bool(cells[x][y])) { thisRow = thisRow | oneBit; }
      if (serialDebugging) { debugLine += bool( thisRow & oneBit ) ? "1 " : "0 "; }
      oneBit = oneBit << 1;
    }
    if (serialDebugging) { Serial.println(debugLine); }

    // Transfer constructed byte for column x to the LED matrix
    Write_Max7219(x+1, thisRow);
  }

}

