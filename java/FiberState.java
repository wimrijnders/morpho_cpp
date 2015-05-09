/*
Copyright 2009, 2010 Snorri Agnarsson, Hallgrímur H. Gunnarsson
Licensed under the Apache License, Version 2.0 (the "License");
                                              15
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
class FiberState
{
    public int pc;
    public ActivationRecord ar;
    public Object acc;
    public final Process proc;
    private java.util.HashMap<Integer,Object> theVarTable = null;
    public FiberState( Process p )
    {
        proc = p;
    }
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
    public final Process getProcess()
    {
        return proc;
    }
}
