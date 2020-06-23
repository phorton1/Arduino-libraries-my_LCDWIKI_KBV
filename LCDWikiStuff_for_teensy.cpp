
#ifdef __MK66FX1M0__
    // only compiles on Teensy 

#include "Arduino.h"
#include "myDebug.h"

// This file should be move to the LCDWIKI_KBV library

#define DEBUG_LCD_STUFF  0
    // defines the teensy pins used, respectively,
    // for the TFT data pins 0..7


uint8_t use_pins[] = {19,13,34,35,36,37,38,39};
    // arbitrary mapping from my teensyExpression project

// method to allow you to set them at runtime
// before any operations are performed

void setTFTDataPins(
        int p0,
        int p1,
        int p2,
        int p3,
        int p4,
        int p5,
        int p6,
        int p7)
{
    use_pins[0] = p0;
    use_pins[1] = p1;
    use_pins[2] = p2;
    use_pins[3] = p3;
    use_pins[4] = p4;
    use_pins[5] = p5;
    use_pins[6] = p6;
    use_pins[7] = p7;
}


void  setReadDir()
{
    #if DEBUG_LCD_STUFF
        display(0,"setReadDir()",0);
    #endif
    for (int i=0; i<8; i++)
        pinMode(use_pins[i],INPUT_PULLUP);
}

void  setWriteDir()
{
    #if DEBUG_LCD_STUFF
        display(0,"setWriteDir()",0);
    #endif
    for (int i=0; i<8; i++)
        pinMode(use_pins[i],OUTPUT);
}


#define USE_FAST_DIGITAL  1
    // makes it about 10-20% faster
    

void _write8(uint8_t v)
{
    #if DEBUG_LCD_STUFF
        display(0,"_write8(0x%02x)",v);
    #endif
    for (int i=0; i<8; i++)
    {
        #if USE_FAST_DIGITAL
            digitalWriteFast(use_pins[i],v & 1);
        #else
            digitalWrite(use_pins[i],v & 1);
        #endif
        v >>= 1;
    }
}        

uint8_t _read8()
{
    uint8_t v = 0;
    for (int i=0; i<8; i++)
    {
        #if USE_FAST_DIGITAL
            v |= digitalReadFast(use_pins[i]) << i;
        #else
            v |= digitalRead(use_pins[i]) << i;
        #endif
    }
    #if DEBUG_LCD_STUFF
        display(0,"read8()=0x%02x",v);
    #endif
    return v;
}



//---------------------------------------------------------------
// (failed) attempt at PORT operations for speed
//---------------------------------------------------------------

#ifdef USE_8BIT_OPERATIONS

    // NOTE THAT THIS (a) DID NOT WORK AS EXPECTED and (b) WAS NOT FASTER
    // For some reason PORTD still output on on pins 1..8 so maybe I don't
    // understand there is another level of indirection in the table below.
    
    // Original Notes
    // Each port (8 pin) has 3 registers.
    // The "x" on the end is replaced by the actual port letter. For example,
    // DDRC controls the direction of pins C0 through C7.
    // 
    //      Register	    Function
    //      DDRx	        Configure Direction: 0=Input, 1=Output
    //      PORTx	        Set Output (when DDRx=1): 0=Low Output, 1=High Output
    //                      Config          Input (when DDRx=0): 0=Normal, 1=Pullup Resistor
    //      PINx	        Read the pin


    uint8_t TEENSY_32_PORT_D_PIN_DATA[] = {2,14,7,8,6,20,21,5};
    uint8_t TEENSY_36_PORT_D_PIN_DATA[] = {1,14,7,8,6,20,21,5};
        // These 8 pins make up 8 contiguous bits at front of PORTD.
        // Note that 3.2 and 3.6 are different by one pin according
        // to table below.


    void  setReadDir()
    {
        DDRD = 0;           // set input direction
        delay(1);
        PORTD = 0xff;       // set pullup resistors
        delay(1);
    }

    void  setWriteDir()
    {
        DDRD = 0xff;        // set output direction
        delay(1);
    }

    void _write8(uint8_t v)
    {
        PORTD = v;          // write the port
        delay(1);
    }        

    uint8_t _read8()
    {
        return PIND;
        delay(1);
    }
    
    // COMMENTS from my old "rpi to teensy parallel data" thing
    //---------------------------------------------------------------
    // in /junk/maybe_wave/Arduino/DMATest/DMATest.ino & prh_readme.md
    //
    
    // Teensy Pin Reference
    // Derived from core_pins.h
    // TeensyPin to PortPin   
    //
    //          3.2?
    //			3.0			3.6
    //   pin   teensy	   teensy
    //    0  	B 16        B 16     
    //    2  	D 0         B 17   
    //    1  	B 17        D 0   
    //    3  	A 12        A 12   
    //    4  	A 13        A 13   
    //    5  	D 7         D 7   
    //    6  	D 4         D 4   
    //    7  	D 2         D 2   
    //    8  	D 3         D 3   
    //    9  	C 3         C 3   
    //    10 	C 4         C 4   
    //    11 	C 6         C 6   
    //    12 	C 7         C 7   
    //    13 	C 5         C 5   
    //    14 	D 1         D 1   
    //    15 	C 0         C 0   
    //    16 	B 0         B 0   
    //    17 	B 1         B 1   
    //    18 	B 3         B 3   
    //    19 	B 2         B 2   
    //    20 	D 5         D 5   
    //    21 	D 6         D 6   
    //    22 	C 1         C 1   
    //    23 	C 2         C 2   
    //    24 	A 5         E 26   
    //    25 	B 19        A 5   
    //    26 	E 1         A 14   
    //    27 	C 9         A 15   
    //    28 	C 8         A 16   
    //    29 	C 10        B 18   
    //    30 	C 11        B 19   
    //    31 	E 0         B 10   
    //    32 	B 18        B 11   
    //    33 	A 4         E 24
    //    34                E 25
    //    35                C 8
    //    36                C 9
    //    37                C 10
    //    38                C 11
    //    39                A 17
    //    40                A 28
    //    41                A 29
    //    42                A 26
    //    43                B 20
    //    44                B 22
    //    45                B 23
    //    46                B 21
    //    47                D 8
    //    48                D 9
    //    49                B 4
    //    50                B 5
    //    51                D 14
    //    52                D 13
    //    53                D 12
    //    54                D 15
    //    55                D 11
    //    56                E 10
    //    57                E 11
    //    58                E 0
    //    59                E 1
    //    60                E 2
    //    61                E 3
    //    62                E 4
    //    63                E 5
    
    
    // attach the clock to the dma.  The following code works
    // with clock on pin3 and a wire between pin2 and pin3
    //
    //    // Configure to trigger DMA of falling edge of signal applied to Pin 2
    //
    //    SIM_SCGC5 |= SIM_SCGC5_PORTD;
    //    CORE_PIN2_CONFIG = PORT_PCR_MUX(1) | PORT_PCR_IRQC(2);
    //    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
    //    SIM_SCGC7 |= SIM_SCGC7_DMA;
    //
    // Analysis:
    //
    //    SIM_SCGC5                     "System Clock Gating Control Register 5"
    //       |= SIM_SCGC5_PORTD;        "Port D Clock Gate Control" == "enabled"
    //
    //    CORE_PIN2_CONFIG =            corepins.h - defined as PORTD_PCR0 (pin control register for port D pin 0 )
    //       PORT_PCR_MUX(1) |          kinetis.h - "Pin Mux Control"
    //                                     1 == "001 Alternative 1 (GPIO)"
    //       PORT_PCR_IRQC(2);          kinetis.h - Interrupt Configuration
    //                                     2 == "0010 ISF flag and DMA request on falling edge"
    //
    //    SIM_SCGC6                     "System Clock Gating Control Register 6"    
    //       |= SIM_SCGC6_DMAMUX;       "DMA Mux Clock Gate Control" == "enabled"
    //
    //    SIM_SCGC7                     "System Clock Gating Control Register 6"
    //      |= SIM_SCGC7_DMA;           "DMA Clock Gate Control" == "enabled"
    
    
    /// example code from Pauls OctoWS2811 driver:
    //
    //      // pin 16 triggers DMA(port B) on rising edge (configure for pin 3's waveform)
    //
    //      CORE_PIN16_CONFIG = PORT_PCR_IRQC(1)|PORT_PCR_MUX(3);
    //      pinMode(3, INPUT_PULLUP); // pin 3 no longer needed
    //
    //      CORE_PIN16_CONFIG =         core pin16 is port B pin 0
    //          PORT_PCR_IRQC(1)        1 == "ISF flag and DMA request on rising edge."
    //        | PORT_PCR_MUX(3);        3 == "Alternative 3 (chip-specific)."
    //      took a while to find this:    == "I2C0_SCL"
    
    // response from someone giving answer to how to set up triggering
    //
    //          PORTA_PCR13 |= PORT_PCR_IRQC(2);
    //              OR
    //          CORE_PIN4_CONFIG = PORT_PCR_IRQC(2)
    //
    //    Enables portA pin13 to generate a DMA request on a falling edge.
    //    Check PORTx_PCRn in the datasheet for details.
    //
    // which appears to connect teensy pin 4, which is PORTA pin 13, to
    // to an interrupt (rising edge?) on pin2
    
    // I am trying to use pins that are out of the way for clocks and handshaking
    // currently have the following:
    //
    //
    //      teensy pin 28 also appears generally available, but is not PWM
    //      so I will use 29
    //
    // so how about we try to create the clock on 25 (PIN_CLOCK_OUT)
    // so that I can see it, and attach that to teensy pin 28, and
    // attach that to the DMA transfer ...
    
#endif // USE_8BIT_OPERATIONS

#endif // __MK66FX1M0__


