# Code Walkthrough


## Quiz Set Up
variables, questions, answers, booleans, time, score.....

each question was stored in `const char questions[10][5]` in the following format storing the question and the possible answers for A, B, C, and D.
```
 {"Who is/are the best TA(s) in this class?",                            
  "A: They both suck", "B: Marco", "C: Liam", "D: We have TAs?"}
```
The answers to each of these questions is stored in the `answer_key()` array, where each of the four values represents the state of each button (A, B, C, D) corresponding to the correct answer.
```
{0, 1, 1, 0}             // Answer is B and C
```


## Interrupts
The interrupt pin is attached to pin 2 due to its compatability with hardware interrupts. The rest of the function was initialized to the `interrupt()` function on the `CHANGE` mode, allowing triggers on both the rising and falling edge of the input: <br>
`attachInterrupt(digitalPinToInterrupt(pin_input_interrupt), interrupt, CHANGE);`

The hardware interrupt is tied to the pin coming out of a 4-input OR gate with the answer buttons as the input, therfore everytime a button is pushed or released, the interrupt is triggered
```
void interrupt(){
  if(secondsCounterAnswer >= time_submit) {                               // Check if button has been held to submit 
    getAnswerFlag = true;}                                                // Set flag to true to allow answer to be checked

  if(end_reset == true){                                                  // Check if reset button is pressed
    if(current_question > 10){                                            // Check if game is over
      end_reset = false;                                                  // Ensure Rest button action is only triggered once
      current_question = 1;                                               // Reset game to the beginnning
      getAnswerFlag = false;                                              // Reset getAnswerFlag to false
      secondsCounterSpent = 0;                                            // Reset counter for time spent on question                        
      secondsCounterAnswer = 0;                                           // Reset everything to beginning states
      right_answer = false;
      question_asked = false;
      end_stats_printed = false;
      led_submit = true;
      score_bonus_counter = 0;                                            // Resets bonus points collected 
      score_correct_counter = 0;                                          // Resets correct answers
      score_total = 0;                                                    // Resets total score value
    }
    
    if(current_question < 10 && current_question > 1){                    // Check if reset buttons is pushed before game is over but passed question 1
      end_reset = false;                                                  // Ensure Rest button action is only triggered once                                         
      current_question = 11;                                              // Ends game
      end_stats_printed = false;                                          // Triggers end stats to be printed 
    }
  }
}
```


## Timer Implementation
The Timer was designed to increment every second.  This allows the rest of our code to be based off of a seconds counter running in the background. For example, the quiz time length is 30 seconds, the button pressing time required to submit an answer is 2 seconds, the time taken to answer the question is recorded in seconds, etc. <br>
This was implemented by manipulating the OCR1A Compare Match value. <br>
OCR1A calculation: {[ CLKSpeed / (Prescaler x DesiredInterruptSpeed) ] -1} : Where the Arduino clock speed (16 MHz), a prescaler of 1024, and setting desired frequency to 1. <br>
The prescaler was set to 1024 by setting bits CS10 and CS12 to 1, corresponding to an encoding of 101. From the data sheets, we know "101" to be the encoding for a 1024 prescaler. This means that the timer will increment once for every 1024 clock cycles of the Arduino system clock. <br>
The interrupt was enabled by setting the Waveform Generation Mode to "Clear Timer on Compare Match" (1 << WGM12). This clears the timer and restarts every time the top value (determined by OCR1A) is reached. <br>
TIMSK1 is the register that controls enabling of interrupts on compare match. By setting OCIE1A to one, it enables the interrupt to trigger when a match occurs OCR1A value. <br>

```
  TCCR1A = 0; TCCR1B = 0;              // Clear Timer 1 control registers to ensure a clean setup
  TCNT1 = 0;                           // Reset Timer 1 counter to 0
  OCR1A = 15624;                       // Set Output Compare Register A for 1-second interrupts (16 MHz / 1024 prescaler)
  TCCR1B |= (1 << WGM12);              // Configure Timer 1 for CTC mode (Clear Timer on Compare Match)
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024 (CS12 and CS10 bits)
  TIMSK1 |= (1 << OCIE1A);             // Enable Timer 1 Compare Match A interrupt                        
```
The Timer interrupt for the code is triggered every second (based off of the OCR1A value). 

```
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
```





### Functions


### Hardware

