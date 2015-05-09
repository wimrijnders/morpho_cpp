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
public class Closure
{
    public int pc;
    public Environment access;
    public int nparams;
    public Operation[] code;
    public Closure(int pc, Environment access, int nparams, Operation[] code)
    {
                                              24
      this.pc = pc;
      this.access = access;
      this.nparams = nparams;
      this.code = code;
  }
  public void call( Process m, Environment params )
  {
      ActivationRecord current = m.getAR();
      ActivationRecord other = new ActivationRecord(current,params,code);
      other.rp = m.getPC();
      other.control = current;
      params.access = this.access;
      m.setAR(other);
      m.setPC(this.pc);
  }
}
