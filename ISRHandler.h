/*
MIT License

Copyright (c) 2017 - Ajay Guleria

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* This class manages interrupt handling. Idea is to free programmer from having
   to think about interrupt handling. Whenever a hardware interrupt occurs, some
   flags will be set for later processing. When user registers an interrupt, user
   also provides a routine that will service it. However, the routine doesn't get
   serviced until the code is back out of ISR. Outside the ISR, a global function
   is provided that will service all the pending interrupts. This function can then
   be used inside each loop so the interrupts don't have to wait for processing
   after code is back to main loop.
*/

// Provide ISR condition and function
class ISRHandler {

public:
    Status registerISR(condition / pin, functionPtr);
    Status unregisterISR(pin);

    // Function to use for processing ISRs in the loop. It will always return true but will
    // take time to process ISRs
    static bool processISRs();
}


