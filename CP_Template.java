import java.io.*;
import java.util.*;

public class CP_Template {
    static BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    static PrintWriter out = new PrintWriter(System.out, true);
    
    public static void main(String[] args) throws IOException {
        int t = Integer.parseInt(br.readLine());
        while (t-- > 0) {
            solve();
        }
        out.flush();
        br.close();
    }
    
    static void solve() throws IOException {
        int n=readInt();
        int k=readInt();
        int[] arr=readIntArray();

        Arrays.sort(arr);
        out.println(n+k);

    }
    
    // Utility methods
    static int readInt() throws IOException {
        return Integer.parseInt(br.readLine());
    }
    
    static long readLong() throws IOException {
        return Long.parseLong(br.readLine());
    }
    
    static String readLine() throws IOException {
        return br.readLine();
    }
    
    static int[] readIntArray() throws IOException {
        String[] tokens = br.readLine().split(" ");
        int[] arr = new int[tokens.length];
        for (int i = 0; i < tokens.length; i++) {
            arr[i] = Integer.parseInt(tokens[i]);
        }
        return arr;
    }
    
    static long[] readLongArray() throws IOException {
        String[] tokens = br.readLine().split(" ");
        long[] arr = new long[tokens.length];
        for (int i = 0; i < tokens.length; i++) {
            arr[i] = Long.parseLong(tokens[i]);
        }
        return arr;
    }
    
    static void printArray(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            if (i > 0) out.print(" ");
            out.print(arr[i]);
        }
        out.println();
    }
    
    static void printArray(long[] arr) {
        for (int i = 0; i < arr.length; i++) {
            if (i > 0) out.print(" ");
            out.print(arr[i]);
        }
        out.println();
    }
}