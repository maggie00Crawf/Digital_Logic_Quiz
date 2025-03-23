//////Pins Definitions//////
const int pin_led_0 = 7;                      // sets pin value for each led0 (far right)
const int pin_led_1 = 6;                      // sets pin value for each led1
const int pin_led_2 = 5;                      // sets pin value for each led2
const int pin_led_3 = 4;                      // sets pin value for each led3 (far left)
const int pin_correct = 12;                   // pin connected to logic comparing input answer with answer_key, HIGH when correct, LOW when wrong
const int pin_input_interrupt = 2;            // pin connectedt to 4-Input OR gate, HIGH when any button is pressed
const int pin_led_submit = 13;                // pin connected to blue light indicating when buttons can be released to submit answer
const int pin_end_reset = 3;                  // pin connected to button that terminates quiz if pushed, same button is also used to restart quiz

const int pin_answerkey_0 = 8;                // pin sets value of A for correrct answer from answer_key combinated 
const int pin_answerkey_1 = 9;                // pin sets value of B for correrct answer from answer_key combinated
const int pin_answerkey_2 = 10;               // pin sets value of C for correrct answer from answer_key combinated
const int pin_answerkey_3 = 11;               // pin sets value of D for correrct answer from answer_key combinated


//////Quiz Parameters//////
const int num_questions = 10;                 // Defines how many questions are in the answer_key
int current_question = 1;                     // Tracks what question you are on

int time_limit_question = 30;                 // Time limit per question in seconds
int time_submit = 2;                          // How long buttons need to be held before answer can be submitted; this is to allow for multiselect 
int time_quiz = 0;                            // Stores total time spent on quiz
int time_avg_question = 0;                    // Stores value of average time spent on a question
int time_bonus = time_limit_question/2;       // Bonus points awarderded if question is answered in half the allotted time

int score_bonus_counter = 0;                  //Keeps track of bonus points collected 
int score_correct_counter = 0;                //Keeps track of correct answers
long score_total = 0.00;                      //holds total score value


//////Answer Key//////
const int answer_key[num_questions][4]={      // Store the button state that equates to the CORRECT answer for each question
  {0, 0, 1, 0},                               // 1st dimentsion = which question number you are on
  {0, 1, 1, 0},                               // 2nd dimension = state of button
  {0, 0, 1, 0},                               // answer_key[current_question-1][0] = Correct state for A
  {0, 1, 0, 0},                               // answer_key[current_question-1][1] = Correct state for B
  {1, 0, 0, 0},                               // answer_key[current_question-1][2] = Correct state for C
  {1, 1, 1, 0},                               // answer_key[current_question-1][3] = Correct state for D
  {0, 0, 1, 1},                               // num_questions parameter = current_question-1 because current_question starts at question 1
  {0, 1, 0, 0}, 
  {0, 1, 0, 1}, 
  {0, 0, 1, 0} 
};

const char* questions[10][5] = {                                          // Array stores the qustions and the answers that will be displayed before the question is answered
  {"What was the first thing we learned in 393L?",                        // 1st dimentsion = which question number you are on
  "A: Logic Gates", "B: MUX", "C: Transistors", "D: Decoders"},           // 2nd dimension = which part of the question is displayed                  
                                                                          // questions[current_question-1][0] = Question
  {"Who is/are the best TA(s) in this class?",                            // questions[current_question-1][1] = Answer Option for A
  "A: They both suck", "B: Marco", "C: Liam", "D: We have TAs?"},         // questions[current_question-1][2] = Answer Option for B                 
                                                                          // questions[current_question-1][3] = Answer Option for C
  {"What is the simplified expression for Y = A + (~A)B?",                // questions[current_question-1][4] = Answer Option for D
  "A: AB", "B: (~A)B", "C: A + B", "D: (~A) + B"},                             
  
  {"Which number system is written in base 16?", 
  "A: Decimal", "B: Hexadecimal", "C: Binary", "D: Alphabet"},                           
  
  {"What type of circuit was created in Lab 2 of 393L?", 
  "A: Multiplier", "B: Adder", "C: Divider", "D: Subtractor"},                   
  
  {"Which of the following is a combinational logic gate?", 
  "A: AND", "B: OR", "C: NOT", "D: Transistor"},                              
  
  {"Which of the following digital pins in the Arduino Uno are usable as Interrupts?", 
  "A: 0", "B: 1", "C: 2", "D: 3"},                 
  
  {"Which 393L group is the best?", 
  "A: Wednesday, Group 10", "B: Monday, Group 1", "C: Thursday, Group 8", "D: Tuesday, Group 13"}, 
  
  {"Which of the following are valid Interrupt modes?", 
  "A: HIGH", "B: CHANGE", "C: SLEEP", "D: LOW"}, 
  
  {"How many bits does a byte consist of?", 
  "A: 4", "B: 1", "C: 8", "D: 16"} 
};


//////Interrupt Parameters//////
volatile int secondsCounterAnswer = 0;              // Tracks how long button has been held, used to submit answer
volatile int secondsCounterSpent = 0;               // Tracks time spent on question

volatile bool limitFlag = false;                    // Flag to indicate when time limit is reached
volatile bool getAnswerFlag = false;                // Flag to indicate when to get the submitted answer
volatile bool right_answer = false;                 // Flag to indicate when correct answer has been submitted
volatile bool question_asked = false;               // Flag to indicate when question needs to be asked
volatile bool end_stats_printed = false;            // Flag to get final performance stats
volatile bool led_submit = false;                   // Flag to indicate when the button has been held long enough 
volatile bool end_reset = false;                    // Flag to indicate when end/reset button has been held for long enough

//////Timer Setup//////
ISR(TIMER1_COMPA_vect) {                            // Defines Interrupt Timer
  secondsCounterSpent++;                            // Increment time spent on question by 1 second
  if(secondsCounterSpent >= time_limit_question){   // Check if time limit has been reached
    limitFlag = true;}                              // Set time limit flag to true if reached
  
  if(digitalRead(pin_input_interrupt) == HIGH){     // Check if any button has been pressed
    if(secondsCounterAnswer >= time_submit){        // Check if button has been held long enough to submit
      led_submit = true;}                           // Flag to indicate blue LED to turn on (button can be released to submit question)
    else{led_submit = false;}                       // If button not held long enough blue should turn off
    secondsCounterAnswer++;}                        // Increment answer counter by 1 while any button stays pushed
    
  if(digitalRead(pin_input_interrupt) == LOW){      // Check that no buttons are pressed
    secondsCounterAnswer = 0;}                      // Reset answer counter since buttons not held 

  if(digitalRead(pin_end_reset) == HIGH){           // Check if end/reset button is pressed
    end_reset = true;                               // Set end reset flag to true if button is pressed
  }  
} 

void setup() {
  Serial.begin(9600);                               // Initialize Serial Monitor at baud rate

  pinMode(pin_led_0, OUTPUT);                       // Initialize led pin numbers as OUTPUT from Arduino Uno
  pinMode(pin_led_1, OUTPUT);
  pinMode(pin_led_2, OUTPUT);
  pinMode(pin_led_3, OUTPUT); 
  pinMode(pin_led_submit, OUTPUT);                  // Initialize pin for indicating buttons pressed long enough as OUTPUT

  pinMode(pin_correct, INPUT);                      // Initialize correct/incorrect answer pin (XNOR+AND gate output) as INPUT
  pinMode(pin_input_interrupt, INPUT);              // Initialize button connection (OR gate output) as INPUT to Arduino
  pinMode(pin_end_reset, INPUT);                    // Initialize end test button as INPUT

  pinMode(pin_answerkey_0, OUTPUT);                 // Initialize answer_key pins as OUTPUTS
  pinMode(pin_answerkey_1, OUTPUT);
  pinMode(pin_answerkey_2, OUTPUT);
  pinMode(pin_answerkey_3, OUTPUT);

  digitalWrite(pin_led_0, HIGH);                    // Turn on all LEDs to begin with 
  digitalWrite(pin_led_1, HIGH);
  digitalWrite(pin_led_2, HIGH);
  digitalWrite(pin_led_3, HIGH);
  digitalWrite(pin_led_submit, HIGH);
  

  ///// TIMER SETUP /////
  noInterrupts();                                   // Disable interrupts during timer setup
  TCCR1A = 0; TCCR1B = 0;                           // Clear Timer 1 Settings
  TCNT1 = 0;                                        // Clear Timer 1 Counter 
  OCR1A = 15624;                                    // Timer set to interrupt every second with a 1024 prescaler (16 MHz clock)
  TCCR1B |= (1 << WGM12);                           // CTC mode (Clear Timer on Compare Match); allows interrupt to be triggered every OCR-set length of time 
  TCCR1B |= (1 << CS12) | (1 << CS10);              // Set prescaler to 1024 by turning on bit 0 and bit 2 of Timer 1B register
  TIMSK1 |= (1 << OCIE1A);                          // Enables interrupts for Compare A matches 
  interrupts();                                     // Re-enable interrupts
  attachInterrupt(digitalPinToInterrupt(pin_input_interrupt),interrupt, CHANGE);     //set "interrupt()" function to run when any button's state has been changed


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){                                       // Run Code
  if(current_question <= 10){                      // For questions 1-10:
    if(question_asked == false){                   // Ask question if not already asked 
      ask_question();
    }
    get_right_answer();                            // Run function to evaluate submitted answer (HIGH for correct and LOW for incorrect)
    get_answer();                                  // Run function to update score, bonus points, etc.
    update_led_timer();                            // Run function to count down remaining time visualized with LEDs
  }   
  if(current_question >10){                        // End quiz once complete
    end_of_quiz_stats();
    while(end_reset == false){      }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void interrupt(){
  if(secondsCounterAnswer >= time_submit) {                               // Check if button has been held to submit 
    getAnswerFlag = true;}                                                // Set flag to true to allow answer to be checked

  if(end_reset == true){                                                  // Check if reset button is pressed
    if(current_question > 10){                                            // Check if game is over
      end_reset = false;
      current_question = 1;                                               // Reset game to the beginnning
      getAnswerFlag = false;                                              // Reset getAnswerFlag to false
      secondsCounterSpent = 0;                                            // Reset counter for time spent on question                        
      secondsCounterAnswer = 0;                                           // Reset everything to beginning states
      right_answer = false;
      question_asked = false;
      end_stats_printed = false;
      led_submit = true;
      score_bonus_counter = 0;                                               // Resets bonus points collected 
      score_correct_counter = 0;                                             // Resets correct answers
      score_total = 0;                                                       // Resets total score value
    }
    
    if(current_question < 10 && current_question > 1){                    // Check if reset buttons is pushed before game is over but passed question 1
      end_reset == false;                                                 // ??                                          
      current_question = 11;                                              // Ends game
      end_stats_printed = false;                                          // Triggers end stats to be printed 
    }
  }
}


void get_right_answer(){                                                   // Function to check if submitted answer is correct
  if(secondsCounterAnswer < time_submit){                                  // Only runs during the time button is pressed down before reaching 2 seconds (so interrupt can be triggered on CHANGE because HIGH is invalid)
    int input_answer = current_question - 1;
    digitalWrite(pin_answerkey_0, answer_key[input_answer][0]);            // Writes the answer of current_question to pins (INPUT that compares with button states to determine if answer is correct)
    digitalWrite(pin_answerkey_1, answer_key[input_answer][1]);            // Feeds correct answer key into AND gate to be compared to submitted answer
    digitalWrite(pin_answerkey_2, answer_key[input_answer][2]);
    digitalWrite(pin_answerkey_3, answer_key[input_answer][3]);
    if(digitalRead(pin_correct) == HIGH){                                  // Signal storing score for current question is HIGH if answer key mathes submitted answer
      right_answer = true;}
    if(digitalRead(pin_correct) == LOW){                                   // Signal storing score for current question is LOW if answer key does not match submitted answer
      right_answer = false;}
  }
}


void print_quiz_stats(){                                                   // Stats displayed after each question 
    Serial.print("The correct answer for question ");                      // Print statement with correct answer
    Serial.print(current_question); Serial.print(" is ");
    for (int i = 0; i < 4; i++) {
      Serial.print(answer_key[current_question-1][i]);                     // Prints each value from answer_key for the current question
      Serial.print(" ");}
    Serial.println();
    Serial.print("Questions answered correctly: ");                        // Prints number of questions answered coreectly so far
    Serial.println(score_correct_counter);
    Serial.print("Your bonus points: ");                                   // Prints number of bonus points accumulated so far
    Serial.println(score_bonus_counter);
    Serial.print("Your total score: ");                                    // Prints total score to date
    Serial.println(score_total);
    Serial.println();
}

void end_of_quiz_stats(){
  while(end_stats_printed == false){                                       // While final stats are not being printed, print them if end_of_quiz stats() is called
    end_stats_printed = true;
    score_total = score_bonus_counter + score_correct_counter;             // Long number that adds total bonus poitns and total correct answers
    long percentage = (score_total * 100) / 20;                            // Performance grade in % 
    int total_time = time_quiz;                                            // Total time taken for whole quiz in seconds 
    time_avg_question = time_quiz/10;                                      // Average time taken per question = total quiz time / number of questions
    int avg_time = time_avg_question;                                      

    Serial.println();                                                      // Print final performance stats
    Serial.print("Your scored "); Serial.print(percentage); Serial.println("% on our quiz!!!");
    Serial.print("You spent ");Serial.print(time_quiz);Serial.println(" seconds on this quiz,");
    Serial.print("With an average time per question of ");Serial.print(avg_time);Serial.println(" seconds.");
    Serial.print("You answered ");Serial.print(score_correct_counter);Serial.print(" questions correctly, ");
    Serial.print("And you earned ");  Serial.print(score_bonus_counter);Serial.println(" bonus points from speedy responses!");
    Serial.println();

    Serial.println("END OF QUIZ");
    Serial.println("Push RED button to reset quiz");
  }
}


void time_is_up(){                              // Function that moves to nextr question if the question timne limit has been reached
  if (limitFlag == true) { 
    time_quiz=time_quiz + time_limit_question;  // Add time limit to total time spent on quiz
    secondsCounterSpent = 0;                    // Reset counter for time spent on question 
    limitFlag = false;                          // Reset time limti flag to false
    print_quiz_stats();
    current_question++;                         // Move to next question
    question_asked = false;                     // Triggers question to be asked
  }
} 

void ask_question(){                                        // Function to print question and answer options
  Serial.print("Question "); Serial.print(current_question); 
  Serial.print(": ");
  for (int i = 0; i < 5; i++) {                             // Iterate through question and each of 4 answers
    Serial.println(questions[current_question-1][i]);       // Print each element on a new line
  }
  Serial.println("");
  question_asked = true;                                    // Prevents question from being asked again
}


void set_led(int led0, int led1, int led2, int led3){       // Takes binary input (led0 = 1 or 0) to set leds
  digitalWrite(pin_led_0, led0);                            // Updates state of LED output due to function input values
  digitalWrite(pin_led_1, led1);
  digitalWrite(pin_led_2, led2);
  digitalWrite(pin_led_3, led3);
}

void reset_answer_pins(){
  digitalWrite(pin_answerkey_0, LOW);           // Sets pins for answer_key values back to 0
  digitalWrite(pin_answerkey_1, LOW);
  digitalWrite(pin_answerkey_2, LOW);
  digitalWrite(pin_answerkey_3, LOW);
}

void update_led_timer() {
  if(secondsCounterSpent < time_limit_question) {                         // LED timer runs while time limit is not up
    if (secondsCounterSpent <= time_limit_question * 0.25) {              // During 1st quarter of time limit
      set_led(1, 1, 1, 1);}                                               // All LEDs are on
    else if (secondsCounterSpent <= time_limit_question * 0.5) {          // During 2nd quarter of time limit
      set_led(1, 1, 1, 0);}                                               // 3 LEDs are on
    else if (secondsCounterSpent <= time_limit_question * 0.75) {         // During 3rd quarter of time limit
      set_led(1, 1, 0, 0);}                                               // 2 LEDs are on
    else {                                                                // During last quarter of time limit
      set_led(1, 0, 0, 0);}                                               // 1 lED is on
  }
  if(secondsCounterSpent == time_limit_question){                         // Once time limit is reached
    set_led(0, 0, 0, 0);}                                                 // All LEDs are off

  if(led_submit == true) {                                               // Check if button has been held long enouhg to submit answer
    digitalWrite(pin_led_submit, HIGH);}                                 // turn indicator light on
  if(led_submit == false) {                               
    digitalWrite(pin_led_submit, LOW);}                                  // turn indicator light off
  }


void get_answer(){
  if (getAnswerFlag == true){                                              // Run function only when button has been held 
    if(right_answer == true){                                              // Check if question is correct 
      score_correct_counter++;                                             // Add 1 to score
      if(secondsCounterSpent < (time_limit_question/2)){                   // Check if time spent on question is less than half the limit
        score_bonus_counter++;}}                                           // Add 1 to bonus socre
    
    time_quiz = time_quiz + secondsCounterSpent;                           // Add time spent on current question to total time spent on quiz
    score_total = score_correct_counter + score_bonus_counter;             // Update total socre 
    print_quiz_stats();                                                    // Prints correct answer for current question, questions answered correctly, bonus points, and total score to date
    reset_answer_pins();                                                   // Sets answer_key pins back to 0 while not answer input is being taken
    current_question++;                                                    // Moves on to next question
    getAnswerFlag = false;                                                 // Reset getAnswerFlag to false
    secondsCounterSpent = 0;                                               // Reset counter for time spent on question                        
    secondsCounterAnswer = 0;                                              // Resets counter for button pressed 
    right_answer = false;                                                  // Resets right answer to false
    question_asked = false;                                                // Triggers next question to be asked 
    led_submit = true;                                     // should this not be flase if we are resetting everything?
  } 
  time_is_up();                                                            // Checks if time limit has been reached, if so, moves to next question
}

