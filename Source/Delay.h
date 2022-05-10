#pragma once

/**
 Base delay class. Call setSize() and setDelayTime() before use
 */

class Delay
{
public:
    
    ///write a sample into the delay line and return the delayed value
    float process (float inputSample)
    {
        
        float output = readVal();
        writeVal (inputSample);
        
        return output;
    }
    
    float readVal()
    {
        // get current value at readPos
        float outVal = buffer [readPos];
        // increment readPos
        readPos ++;
        
        if (readPos >= size)
            readPos = 0;
        
        return outVal;
            
    }
    
    void writeVal (float inputSample)
    {
        // store current value at writePos
        buffer [writePos] = inputSample;
        //increment writePos
        writePos ++;
        
        if (writePos >= size)
            writePos = 0;
    }
    
    ///set the size of the delay line in Samples
    void setSize (int sizeInSamples)
    {
        size = sizeInSamples;
        
        buffer = new float [size];
        
        // initialise all values to zero 
        for (int i=0; i<size; i++)
        {
            buffer[i] =0.0;
        }
    }
    
    
    ///Set the delay time in Samples
    void setDelayTime (int _delayTimeInSample)
    {
        delayTimeInSamples = _delayTimeInSample;
        
        if  (delayTimeInSamples > size)
            delayTimeInSamples = size;
        
        readPos = writePos - delayTimeInSamples;
        
        // if readPos < 0, then add size to readPos
        if (readPos < 0)
            readPos += size;
    }
    
    //void setFeedback (float _fb)
   // {
      //  feedback = _fb;
        
      //  if (feedback > 1.0)
      //      feedback = 1.0;
        
      //  if (feedback < 0.0)
      //     feedback = 0.0;
            

    
private:
    float * buffer; // store samples
    int size; // size of buffer in samples
    int delayTimeInSamples;
    int readPos = 0; // read position
    int writePos = 0; // write position 

};


