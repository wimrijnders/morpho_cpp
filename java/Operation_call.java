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
                                             18
import java.io.InputStream;
import java.io.OutputStream;
public class Operation_Call extends Operation
{
    private int offset;
    private int parpos;
    public void setArg(int i, Comparable o)
    {
        switch (i) {
            case 0:
                this.offset = (Integer)o;
                break;
            case 1:
                this.parpos = (Integer)o;
                break;
        }
    }
    public int argCount()
    {
        return 2;
    }
    public Comparable getArg( int i )
    {
        switch(i)
        {
            case 0: return offset;
            case 1: return parpos;
            default:
                     throw new Error("Out of bounds");
        }
    }
    public void execute(Process m) throws Die, Yield, Exception
    {
        ActivationRecord current = m.getAR();
        Environment params = parpos==0 ? null : (Environment)current.get(parpos);
        if (offset < -1000000000) {
            int n = -offset-1000000000;
            try
            {
                m.setAcc(Builtins.execute(m, params, n));
                return;
            }
            catch( Yield e )
            {
                throw e;
            }
            catch( Exception e )
            {
                current.function_name = "Builtin operation "+Builtins.getOpName(n);
                current.line = 0;
                throw e;
            }
            catch( Error e )
            {
                current.function_name = "Builtin operation "+Builtins.getOpName(n);
                current.line = 0;
                throw e;
            }
        }
        if( params==null )
        {
            params = new Environment();
        }
        ActivationRecord other = new ActivationRecord(current,params,m.getCode());
                                              19
        other.rp = m.getPC();
        other.control = current;
        m.setAR(other);
        m.setPC(m.getPC() + offset);
        if( parpos!=0 ) current.put(parpos, null);
    }
}
