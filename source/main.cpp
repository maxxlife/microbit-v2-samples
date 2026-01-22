#include "MicroBit.h"
#include "LightLevelDetector.h"

MicroBit uBit;
LightLevelDetector *lightSensor;

bool isBoxOpen = false;

// Event handlers
void onLidOpen(Event)
{
    DMESG("LID OPEN: Playing Music");
    uBit.display.print("O"); // Visual debug
    isBoxOpen = true;
}

void onLidClose(Event)
{
    DMESG("LID CLOSED: Stopping Music");
    uBit.display.clear();
    isBoxOpen = false;
}

// Frequencies for Pop Goes the Weasel
#define C5 523
#define D5 587
#define E5 659
#define G5 784

// Helper to play a tone using the CODAL audio pipeline
void playTone(int frequency, int duration_ms)
{
    // 1.Set the pitch (Period=1 second/frequency)
    uBit.audio.virtualOutputPin.setAnalogPeriodUs(1000000 / frequency);
    
    // 2. Turn on volume (50% duty cycle square wave)
    uBit.audio.virtualOutputPin.setAnalogValue(512);
    
    // 3. Wait for the note duration
    uBit.sleep(duration_ms);
    
    // 4. Turn off sound
    uBit.audio.virtualOutputPin.setAnalogValue(0);
}

void playMusicBox()
{
    while (true)
    {
        if (isBoxOpen)
        {
            // Melody arrays
            int melody[] = {C5,  C5,  D5,  D5,  E5,  G5,  E5,  C5};
            int beats[]  = {400, 200, 400, 200, 400, 600, 200, 800};

            for (int i = 0; i < 8; i++)
            {
                if (!isBoxOpen) break;

                playTone(melody[i], beats[i]);
                
                // Tiny gap between notes so they don't blur together
                uBit.sleep(50); 
            }
            
            // Long pause between loops
            if (isBoxOpen) uBit.sleep(1000);
        }
        else
        {
            // If closed, yield to other fibers and save power
            uBit.sleep(200);
        }
    }
}

int main()
{
    uBit.init();

    //eable the internal speaker and set max volume
    uBit.audio.setSpeakerEnabled(true);
    uBit.audio.setVolume(255);

    // initial test beep, so to prove speaker works on boot
    playTone(880, 200); 

    // instatiate driver
    lightSensor = new LightLevelDetector(uBit.display);

    uBit.messageBus.listen(MICROBIT_ID_LIGHT_DETECTOR, MICROBIT_LIGHT_EVT_LIGHT, onLidOpen);
    uBit.messageBus.listen(MICROBIT_ID_LIGHT_DETECTOR, MICROBIT_LIGHT_EVT_DARK, onLidClose);

    create_fiber(playMusicBox);
    release_fiber();
}