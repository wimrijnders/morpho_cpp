/*
Copyright 2009, 2010 Snorri Agnarsson, Hallgrímur H. Gunnarsson
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
package is.hi.cs.morpho;
import java.util.LinkedList;
import java.util.Deque;
import java.util.concurrent.LinkedBlockingDeque;
public class Process implements Runnable
{
    private int pc;
    private ActivationRecord ar; // Current activation record
    private Operation[] code; // Current code array, same as ar.code
    private Object acc; // Current accumulator
    private volatile FiberState current; // Current fiber
    private final LinkedList<FiberState> ready; // Current queue of ready fibers
    private boolean dead = false; // True iff this process has been killed and should stop
    private final Machine m;
    public ActivationRecord theLastTransferSourceActivationRecord;
    // Only guaranteed valid from transfer/throw
    // until next transfer/throw or next end of
    // catch block
    public Process( Machine m )
    {
        ready = new LinkedList<FiberState>();
        //ready = new LinkedBlockingDeque<FiberState>();
        //ready = new ConcurrentLinkedQueue<FiberState>();
        this.pc = 0;
        this.ar = null;
        this.m = m;
    }
    public final Machine getMachine()
    {
        return m;
    }
    public synchronized final boolean isDead()
    {
                                             11
    return dead;
}
public synchronized final void kill()
{
    dead = true;
}
public Operation[] getCode()
{
    return code;
}
public synchronized void loadFiber(FiberState p)
{
    this.ar = p.ar;
    this.pc = p.pc;
    this.acc = p.acc;
    this.code = p.ar.code;
}
public void add(FiberState f)
{
    synchronized(ready)
    {
        ready.offer(f);
        ready.notifyAll();
    }
    m.add(this);
}
public synchronized FiberState getCurrent()
{
    return current;
}
public synchronized void setCurrent( FiberState s )
{
    current = s;
}
public int getNumReady()
{
    synchronized(ready) {return ready.size();}
}
public synchronized void saveFiber(FiberState p)
{
    p.ar = this.ar;
    p.pc = this.pc;
    p.acc = this.acc;
}
public void run()
{
    try
    {
        startNicely();
    }
    catch( InterruptedException e )
    {
        throw new Error(e);
    }
}
public final Object start() throws InterruptedException
{
    for(;;)
    {
        synchronized(this)
        {
            if( dead ) return acc;
                                         12
            synchronized(ready) {current = ready.poll();}
            while( current==null )
            {
                wait();
                synchronized(ready) {current = ready.poll();}
            }
            loadFiber(current);
        }
        try
        {
            for (;;)
            {
                code[pc++].execute(this);
            }
        }
        catch( KillMachine e )
        {
            kill();
            m.kill();
            return acc;
        }
        catch( Die e )
        {
            kill();
            return acc;
        }
        catch( Throwable e )
        {
            synchronized(this)
            {
                FiberState exception_continuation =
                     this.ar==null ? null :
                                     this.ar.exception_continuation;
                if( exception_continuation == null )
                {
                     System.err.println("No exception handler.");
                     System.err.println("PC="+pc);
                     kill();
                     throw new Error(e);
                }
                theLastTransferSourceActivationRecord = this.ar;
                this.acc = e;
                this.pc = exception_continuation.pc;
                this.ar = exception_continuation.ar;
                this.code = exception_continuation.ar.code;
                if( current!=null )
                {
                     saveFiber(current);
                     synchronized(ready) {ready.offer(current);ready.notifyAll();}
                }
            }
        }
    }
}
public final synchronized void startNicely() throws InterruptedException
{
    int n=0;
    for(;;)
    {
        if( dead ) return;
        synchronized(ready) {current = ready.poll();}
        if( current==null ) return;
        loadFiber(current);
        try
        {
            for (;;)
            {
                if( n++ > 100000 )
                {
                     saveFiber(current);
                                          13
                     synchronized(ready) {ready.addFirst(current);}
                     m.add(this);
                     return;
                }
                code[pc++].execute(this);
                if( current==null )
                {
                     if( dead ) return;
                     m.add(this);
                     return;
                }
            }
        }
        catch( KillMachine e )
        {
            kill();
            m.kill();
            return;
        }
        catch( Die e )
        {
            kill();
            return;
        }
        catch( Throwable e )
        {
            FiberState exception_continuation =
                this.ar==null ? null :
                                  this.ar.exception_continuation;
            if( exception_continuation == null )
            {
                System.err.println("No exception handler.");
                System.err.println("PC="+pc);
                kill();
                throw new Error(e);
            }
            theLastTransferSourceActivationRecord = this.ar;
            this.acc = e;
            this.pc = exception_continuation.pc;
            this.ar = exception_continuation.ar;
            this.code = exception_continuation.ar.code;
            if( current!=null )
            {
                saveFiber(current);
                synchronized(ready) {ready.offer(current);ready.notifyAll();}
            }
        }
    }
}
public synchronized final void setPC(int location)
{
    this.pc = location;
}
public synchronized final int getPC()
{
    return this.pc;
}
public synchronized final ActivationRecord getAR()
{
    return this.ar;
}
public synchronized final Object getAcc()
{
    return this.acc;
}
public synchronized final void setAcc(Object o)
                                          14
    {
        this.acc = o;
    }
    public synchronized final void setAR(ActivationRecord ar)
    {
        this.ar = ar;
        if( ar!=null && ar.code != null )
            this.code = ar.code;
        else
            throw new Error("AR invalid");
    }
    public synchronized Object getControlTrace()
    {
        ActivationRecord tmp = theLastTransferSourceActivationRecord;
        int n=0;
        while( tmp != null )
        {
            n++;
            tmp = tmp.control;
        }
        Object[] trace = new Object[n];
        int i=0;
        tmp = ar;
        while( tmp != null )
        {
            Object[] rec = new Object[6]; // [fname,line,file,fnameorg,lineorg,fileorg]
            rec[0] = tmp.function_name;
            rec[1] = tmp.line==0 ? null : (""+tmp.line);
            rec[2] = tmp.filename;
            Operation op = tmp.rp-2 >= 0 ? code[tmp.rp-2] : null;
            if( op instanceof Operation_CallInfo )
            {
                 Operation_CallInfo info = (Operation_CallInfo)op;
                 rec[3] = info.function_name;
                 rec[4] = info.line==0 ? null : info.line;
                 rec[5] = info.file_name;
            }
            trace[i++] = rec;
            //System.out.println(tmp.function_name);
            tmp = tmp.control;
        }
        return trace;
    }
    private java.util.HashMap<Integer,Object> theVarTable = new java.util.HashMap<Integer,Object>();
    public synchronized void storeGlobal( int idx, Object val )
    {
        if( theVarTable==null )
            theVarTable = new java.util.HashMap<Integer,Object>();
        theVarTable.put(idx,val);
    }
    public synchronized Object fetchGlobal( int idx )
    {
        if( theVarTable==null ) return null;
        return theVarTable.get(idx);
    }
}
