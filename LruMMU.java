import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.sql.Time;
//import java.time.Instant;

/**
 * MMU using least recently used replacement strategy
 */

public class LruMMU implements MMU {
    public int PageFaults;
    public int DiskWrites;
    public int DiskReads;

    // stores the PageNumber:time, each time a page is hit, update their time
    public HashMap<Integer, Long> TLB; //could change to just be a counter, would be easier
    public Set<Integer> dirtyPages;


    public int MaxFrames;
    public boolean debug;


    public LruMMU(int frames) {
        PageFaults = 0;
        DiskReads = 0;
        DiskWrites = 0;
        TLB = new HashMap<>();
        dirtyPages = new HashSet<>();
        MaxFrames = frames;
        debug = false;
    }

    

    /*
     * If page is in TLB, updates its access time and return 1
     * if page not in TLB, adds page in, replacing as needed and returns -1
     * if page to be removed is dirty, returns 0
     * increment PageFaults, DiskReads and DiskWrites based on the output of this
     * function
     */
    public int Replace(int page_number) {
        // page hit, update time since it has been used
        if (TLB.containsKey(page_number)) {
            TLB.replace(page_number, System.nanoTime());
            return 1;
        }

        if (TLB.size() < MaxFrames) {
            TLB.put(page_number, System.nanoTime());
            return -1;
        }

        // cache full evict least recently used
        if (TLB.size() == MaxFrames) {
            // find oldest entry
            Long OldestEntry = Long.MAX_VALUE;
            int OldestKey = -1;
            for (Integer keys : TLB.keySet()) {
                Long current = TLB.get(keys);
                if (current < OldestEntry) {
                    OldestEntry = current;
                    OldestKey = keys;
                }
            }

            if (OldestKey != -1) {
                // remove oldest entry
                TLB.remove(OldestKey);

                // add in new page
                TLB.put(page_number, System.nanoTime());
            }
            // remove from dirtypages?
            if (dirtyPages.contains(OldestKey)) {
                dirtyPages.remove(OldestKey);
                return 0;
            }

        }
        return -1;
    }

    public void setDebug() {
        debug = true;
    }

    public void resetDebug() {
        debug = false;
    }

    public void readMemory(int page_number) {
        // todo
        // if(!dirtyPages.contains(page_number))
        // {
        //     dirtyPages.add(page_number);
        // }
        int retVal = Replace(page_number);
        if(retVal == -1)
        {
            PageFaults++;
            DiskReads++;
        }
        else if(retVal == 0)
        {
            PageFaults++;
            DiskReads++;
            DiskWrites++;
        }
    }

    public void writeMemory(int page_number) {
        // insert into DirtyPages here
        dirtyPages.add(page_number);
        int retVal = Replace(page_number);
        if(retVal == -1)
        {
            PageFaults++;
        }
        else if(retVal == 0)
        {
            PageFaults++;
            DiskWrites++;
        }
    }

    public int getTotalDiskReads() {
        // todo
        return DiskReads;
    }

    public int getTotalDiskWrites() {
        // todo
        return DiskWrites;
    }

    public int getTotalPageFaults() {
        return PageFaults;
    }
}