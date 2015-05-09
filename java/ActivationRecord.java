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
public class ActivationRecord
                                             16
{
    public ActivationRecord control;
    public int rp;
    public Environment env;
    public Object[] tmpvars = null;
    public FiberState exception_continuation;
    public Operation[] code;
    // Traceback info, set on entry to function
    public String function_name = null; // name of the function in original source file
    public String filename = null; // name of source file
    public int line = 0; // approximate line where function is defined
    public ActivationRecord( ActivationRecord outer, Environment env, Operation[] code )
    {
        this.env = env;
        if( outer!=null )
            this.exception_continuation = outer.exception_continuation;
        this.code = code;
    }
    public final void put( int n, Object o )
    {
        if (n >= 0) {
            env.put(n, o);
            return;
        }
        n = -1-n;
        if (tmpvars == null) {
            tmpvars = new Object[n<3?3:n+1];
        } else if (tmpvars.length <= n) {
            int k = 2*tmpvars.length;
            if (k <= n)
                k = n+1;
            Object[] newtmp = new Object[k];
            System.arraycopy(tmpvars, 0, newtmp, 0, tmpvars.length);
            tmpvars = newtmp;
        }
        tmpvars[n] = o;
    }
    public final Object get(int n)
    {
        if (n >= 0)
            return env.get(n);
        n = -1-n;
        if (tmpvars == null || tmpvars.length <= n)
            return null;
        return tmpvars[n];
    }
}
