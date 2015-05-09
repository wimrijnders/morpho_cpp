/*
Copyright 2009, 2010 Snorri Agnarsson, Hallgrímur H. Gunnarsson
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
                                             17
You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
package is.hi.cs.morpho;
import java.util.Vector;
public class Environment
{
    public Environment access;
    public Object[] localvars = null;
    public final void put(int n, Object o)
    {
        if (localvars == null) {
            localvars = new Object[n<3?3:n+1];
        } else if (localvars.length <= n) {
            int k = 2*localvars.length;
            if (k <= n)
                k = n+1;
            Object[] newtmp = new Object[k];
            System.arraycopy(localvars, 0, newtmp, 0, localvars.length);
            localvars = newtmp;
        }
        localvars[n] = o;
    }
    public final Object get(int n)
    {
        if (localvars == null || localvars.length <= n)
            return null;
        return localvars[n];
    }
}
