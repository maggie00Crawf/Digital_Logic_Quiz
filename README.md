# Digital_Logic_Quiz
Digital multiple choice quiz game incorporating timers and interrupts 


## Code Walkthrough

### Interrupts
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


### Timer Implementation
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

