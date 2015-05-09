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
import java.util.HashSet;
public class Machine implements Runnable
{
    public static String[] args;
    private final LinkedList<Process> ready; // Current queue of ready processes
    private final HashSet<Process> waiting;
    private volatile boolean dead = false;
    public Machine()
    {
        ready = new LinkedList<Process>();
        waiting = new HashSet<Process>();
    }
    public Machine(Operation[] code, int startPC)
    {
        this();
        Process p = new Process(this);
        FiberState f = new FiberState(p);
        f.ar = new ActivationRecord(null,new Environment(),code);
        f.ar.control = f.ar;
        f.ar.rp = -1;
        f.pc = startPC;
        p.add(f);
        //add(p); previous line does this
        run1();
    }
    public boolean isDead()
    {
        return dead;
    }
    public void kill()
    {
        dead = true;
    }
    public void run()
    {
        run(4);
    }
    public void run( int n )
                                              9
{
    if( n<2 )
    {
        run1();
        return;
    }
    try
    {
        java.util.concurrent.ExecutorService executor =
            java.util.concurrent.Executors.newFixedThreadPool(n);
        for(;;)
        {
            Process p;
            p = getNextReady();
            if( p==null ) break;
            executor.execute(p);
        }
        executor.shutdown();
    }
    catch( InterruptedException e )
    {
        throw new Error(e);
    }
}
public void run1()
{
    try
    {
        for(;;)
        {
            Process p;
            p = getNextReady();
            if( p==null ) break;
            p.run();
        }
    }
    catch( InterruptedException e )
    {
        throw new Error(e);
    }
}
private Process getNextReady() throws InterruptedException
{
    Process res;
    try
    {
        synchronized(this)
        {
            for(;;)
            {
                 if( dead ) return null;
                 if( ready.isEmpty() )
                 {
                     wait();
                     continue;
                 }
                 res = ready.remove();
                 waiting.remove(res);
                 break;
            }
        }
        return res;
    }
    catch( InterruptedException e )
    {
        throw new Error(e);
    }
}
                                         10
    public synchronized void add( Process p )
    {
        if( p.getMachine() != this )
            throw new Error("Attempt to add a process to a machine that does not own the process");
        if( waiting.contains(p) ) return;
        waiting.add(p);
        ready.offer(p);
        notifyAll();
    }
}
