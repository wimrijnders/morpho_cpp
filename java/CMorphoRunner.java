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
import java.io.InputStream;
import java.util.zip.InflaterInputStream;
import java.io.DataInput;
import java.io.FileInputStream;
import java.io.DataInputStream;
import java.io.DataOutput;
import java.io.FileOutputStream;
import java.io.DataOutputStream;
public class CMorphoRunner
{
    static public Module getBasis()
        throws Exception
    {
        InputStream in = null;
        DataInputStream din = null;
        try{
            in = new CMorphoRunner().getClass().getClassLoader().getResourceAsStream("basis.mmod");
            in = new InflaterInputStream(in);
            din = new DataInputStream(in);
        }
        catch(Exception e)
        {
            e.printStackTrace();
            throw e;
        }
        Module res = new Module();
        res.load(din);
        din.close();
        return res;
    }
    static public void compile( String[] args )
        throws Exception
    {
        Loader.registerOps();
        Operation[] code = null;
        int pc;
        DataInputStream in=null;
        try
        {
            in = new DataInputStream(
                                             22
                  new InflaterInputStream(
                      new CMorphoRunner().getClass()
                                         .getClassLoader()
                                         .getResourceAsStream("cmorpho.mexe")
                  )
              );
        Loader loader = new Loader(in);
        pc = loader.loadInt();
        int n = loader.loadInt();
        code = new Operation[n];
        for( int i=0 ; i!=n ; i++ )
        {
            byte typecode = loader.loadByte();
            if( typecode == -128 )
            {
                 code[i] = loader.loadOperation();
            }
            else
            {
                 code[i] = code[loader.loadInt()];
            }
        }
    }
    finally
    {
        if(in==null)
        {
            System.err.println("cmorpho.mexe resource not found");
        }
        else
        {
            in.close();
        }
    }
    String[] newArgs = new String[args.length-1];
    System.arraycopy(args,1,newArgs,0,args.length-1);
    Machine.args = newArgs;
    Machine m = new Machine(code, pc);
}
static public void main( String[] args ) throws Exception
{
    if( args.length < 1 ||
        args[0].equals("--help") ||
        args[0].equals("-h") ||
        args[0].equals("-?") )
    {
        System.err.println("Usage:");
        System.err.println(" morpho <name> <arg>...");
        System.err.println("Or:");
        System.err.println(" java -jar morpho.jar <name> <arg>...");
        System.err.println("Or:");
        System.err.println(" morpho [--compile|-c] [<opt>|<filename>]...");
        System.err.println("Or:");
        System.err.println(" java -jar morpho.jar [--compile|-c] [<opt>|<filename>]...");
        System.err.println("    where <opt> = [--asm|--noasm|--debug|--nodebug|--encoding NAME]");
        System.err.println("Or:");
        System.err.println(" morpho [--help|-h|-?]");
        System.err.println("Or:");
        System.err.println(" java -jar morpho.jar [--help|-h|-?]");
        System.exit(1);
    }
    if( args[0].equals("--compile") || args[0].equals("-c") )
    {
        compile(args);
        return;
    }
    Loader.registerOps();
    Operation[] code = null;
    int pc;
    DataInputStream in=null;
                                          23
        try
        {
            in = new DataInputStream(
                  new java.util.zip.InflaterInputStream(
                  new FileInputStream(args[0]+".mexe")));
            Loader loader = new Loader(in);
            pc = loader.loadInt();
            int n = loader.loadInt();
            code = new Operation[n];
            for( int i=0 ; i!=n ; i++ )
            {
                byte typecode = loader.loadByte();
                if( typecode == -128 )
                {
                     code[i] = loader.loadOperation();
                }
                else
                {
                     code[i] = code[loader.loadInt()];
                }
            }
        }
        finally
        {
            if(in==null)
            {
                System.err.println("File "+args[0]+".mexe"+" not found");
            }
            else
            {
                in.close();
            }
        }
        Machine.args = args;
        Machine m = new Machine(code, pc);
    }
}
