import java.util.concurrent.*;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;
import java.util.ArrayList;


class Worker implements Runnable{
	long sum=0;
	Tree tree;
	TreeCalculation tc;
	
	public Worker( TreeCalculation tc ) {
		this.tc = tc;
		tc.accessLock.lock();
		tc.availableWorkers.add(this);
		tc.availableWorkersNotEmpty.signal();
		tc.accessLock.unlock();
	}
	
	public long getSum(){
		return sum;
	}

	void setTree(Tree tree) {
		this.tree = tree;
	}
	
	void reset() {
		sum = 0;
		tree = null;
	}
	
	@Override
	public void run() {
		//System.out.printf("Starting worker from tree node %d\n", tree.value);
		long sum = tree.processTree();
		//System.out.printf("Worker from tree node %d got sum %d\n", tree.value, sum);
		tc.accessLock.lock();
		tc.completionService.add( sum );
		reset();
		tc.availableWorkers.add(this);
		tc.availableWorkersNotEmpty.signal();
		tc.decrementTasks();
		tc.accessLock.unlock();
	}
}

class CompletionService{
	long sum = 0;
	
	void add( long sum ) {
		this.sum += sum;
	}
	
	long getSum() {
		return sum;
	}
}

public class TreeCalculation {
    // tree level to go parallel
    int levelParallel;
    // total number of generated tasks
    long totalTasks;
    // current number of open tasks
    long nTasks;
    // total height of tree
    int height;
    
    ReentrantLock accessLock = new ReentrantLock();
    final Condition availableWorkersNotEmpty = accessLock.newCondition(); 
    
    CompletionService completionService = new CompletionService();

    ArrayList<Worker> availableWorkers = new ArrayList<Worker>();
    
    TreeCalculation(int height, int levelParallel) {
	this.height = height;
	this.levelParallel = levelParallel;
    }

    void incrementTasks() {
	++nTasks;
	++totalTasks;
    }

    void decrementTasks() {
	--nTasks;
    }

    long getNTasks() {
	return nTasks;
    }

    void preProcess(int threadCount) {
    	for(int i=0; i<threadCount; i++) {
    		new Worker(this);
    	}
    }

    long postProcess() {
    	long nTasks = 0;
    	do {
    		accessLock.lock();
    		nTasks = getNTasks();
    		accessLock.unlock();
    	}
    	while(  nTasks != 0 );
    	
    	accessLock.lock();
    	long result = completionService.getSum();
    	accessLock.unlock();
    	
    	return result;
    }

    public static void main(String[] args) {
	if (args.length != 3) {
	    System.out.println("usage: java Tree treeHeight levelParallel nthreads\n");
	    return;
	}
	int height = Integer.parseInt(args[0]);
	int levelParallel = Integer.parseInt(args[1]);
	int threadCount = Integer.parseInt(args[2]);

	TreeCalculation tc = new TreeCalculation(height, levelParallel);

	// generate balanced binary tree
	Tree t = Tree.genTree(height, height);

	System.gc();

	// traverse sequential
	long t0 = System.nanoTime();
	long p1 = t.processTree();
	double t1 = (System.nanoTime() - t0) * 1e-9;

	t0 = System.nanoTime();
	tc.preProcess(threadCount);
	long p2 = t.processTreeParallel(tc);
	p2 += tc.postProcess();
	double t2 = (System.nanoTime() - t0) * 1e-9;

	long ref = (Tree.counter * (Tree.counter + 1)) / 2;
	if (p1 != ref)
	    System.out.printf("ERROR: sum %d != reference %d\n", p1, ref);
	if (p1 != p2)
	    System.out.printf("ERROR: sum %d != parallel %d\n", p1, p2);
	if (tc.totalTasks != (2 << levelParallel)) {
	    System.out.printf("ERROR: ntasks %d != %d\n", 2 << levelParallel, tc.totalTasks);
	}

	// print timing
	System.out.printf("tree height: %2d sequential: %.6f parallel with %3d threads and %6d tasks: %.6f  speedup: %.3f count: %d\n",
			  height, t1, threadCount, tc.totalTasks, t2, t1 / t2, ref);
    }
}


// ==============================================================================

class Tree {

    static long counter; // counter for consecutive node numbering

    int level; // node level
    long value; // node value
    Tree left; // left child
    Tree right; // right child

    // constructor
    Tree(long value) {
	this.value = value;
    }

    // generate a balanced binary tree of depth k
    static Tree genTree(int k, int height) {
	if (k < 0) {
	    return null;
	} else {
	    Tree t = new Tree(++counter);
	    t.level = height - k;
	    t.left = genTree(k - 1, height);
	    t.right = genTree(k - 1, height);
	    return t;
	}
    }

    // ==============================================================================
    // traverse a tree sequentially

    long processTree() {
	return value
	    + ((left == null) ? 0 : left.processTree())
	    + ((right == null) ? 0 : right.processTree());
    }

    // ==============================================================================
    // traverse a tree parallel
    
    long processTreeParallel(TreeCalculation tc) {
    	
    	if(level==tc.levelParallel) {
    		tc.accessLock.lock();
    		while(tc.availableWorkers.isEmpty())
				try {
					//System.out.printf("Waiting for a worker for left tree\n");
					tc.availableWorkersNotEmpty.await();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
    		//System.out.printf("Got a worker for left tree\n");
    		Worker leftWorker = tc.availableWorkers.remove(0);
    		tc.incrementTasks();
    		tc.accessLock.unlock();
    		leftWorker.setTree(left);
    		Thread leftWorkingThread = new Thread(leftWorker);
    		leftWorkingThread.start();
    		
    		tc.accessLock.lock();
    		while(tc.availableWorkers.isEmpty())
				try {
					//System.out.printf("Waiting for a worker for right tree\n");
					tc.availableWorkersNotEmpty.await();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
    		//System.out.printf("Got a worker for right tree\n");
    		Worker rightWorker = tc.availableWorkers.remove(0);
    		tc.incrementTasks();
    		tc.accessLock.unlock();
    		rightWorker.setTree(right);
    		Thread rightWorkingThread = new Thread(rightWorker);
    		rightWorkingThread.start();
    		
    		//System.out.printf("Adding sequential %d\n", value);
    		return value;
    	}else {
    		//System.out.printf("Adding sequential %d\n", value);
    		return value
    			    + ((left == null) ? 0 : left.processTreeParallel(tc))
    			    + ((right == null) ? 0 : right.processTreeParallel(tc));
    	}
    }
}

//==============================================================================
