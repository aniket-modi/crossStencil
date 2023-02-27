#include "hdf5.h"


/*
    This program reads a slab of data and then takes a userdefined step in every dimension
    and then reads the same sized slab again until it reaches the end in any dimension
*/


// Size of the chunks
#define CHUNK_X 20
#define CHUNK_Y 20

// Size of the file
#define DATASET_X 100
#define DATASET_Y 100

// These will be user inputs for simplicity defining as global variables


int main(int argc, char **argv){

    if (argc != 7) {
      printf("Required arguments: offsetX, offsetY, readSizeX, readSizeY\n");
      return -1;
    }

    // Init the system to default values before taking user inputs 
    int stepx=5;
    int stepy=5;
    int stepz=5;
    int readSizeX=10;
    int readSizeY=10;
    int readSizeZ=10;


    // Read the user inputs    
    sscanf(argv[1], "%d", &stepx);
    sscanf(argv[2], "%d", &stepy);
    sscanf(argv[3], "%d", &stepz);
    sscanf(argv[4], "%d", &readSizeX);
    sscanf(argv[5], "%d", &readSizeY);
    sscanf(argv[6], "%d", &readSizeZ);


    hsize_t dimsf[3]; /* dataset dimensions */
    
    /* 
     * Data  and output buffer initialization. 
     */
    
    hid_t       file, dataset;         /* handles */
    hid_t       dataspace;   
    hid_t       memspace; 
    hsize_t     dimsm[3];              /* memory space dimensions */    
    herr_t      status;
    hsize_t     dims_out[3];  

    int data_out [readSizeX][readSizeY]; /* output buffer */

    hsize_t count[3];        /* Size of data to read from the dataset */
    hssize_t offset[3];      /* Offset of the data to read from the dataset */


    hsize_t count_out[3];        /* Size of data to read into memory */
    hssize_t offset_out[3];      /* Offset of the data to read into memory */

    /*
     * Open the file and the dataset.
     */
    file = H5Fopen ("/crossStencil/mnist.h5", H5F_ACC_RDONLY, H5P_DEFAULT);

    dataset = H5Dopen (file, "images", H5P_DEFAULT);

    dataspace = H5Dget_space (dataset);    /* dataspace handle */

    int rank = H5Sget_simple_extent_ndims (dataspace);
    
    status  = H5Sget_simple_extent_dims (dataspace, dims_out, NULL); // Get the size of each Dimension

    printf("Dimsnout: %lld %lld %lld\n", dims_out[0], dims_out[1], dims_out[2]);

    printf("\nRank: %d\nDimensions: %lu x %lu x %lu \n", rank,
	   (unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]), (unsigned long)(dims_out[2]));
    // Memory dataspace
    // Dataspace is essentially the defintion of the size etc of a dataset
    // or for any raw data.
    // Define the format of the data chubk we want to read
    // The size, offset etc
    dimsm[0] = readSizeZ;
    dimsm[1] = readSizeX;
    dimsm[2] = readSizeY;

    // Create a dataspace fot the chunk we waant to read
    memspace = H5Screate_simple (rank, dimsm, NULL);

    // Offset into the dataset we are reading into
    // Here it is 0 as the size of what we are reading
    // and what we are storing it in is the same
    offset_out[0] = 0;
    offset_out[1] = 0;   
    offset_out[2] = 0;

    // Number of elements to read in each dimension
    count_out[0] = readSizeZ;
    count_out[1] = readSizeX;
    count_out[2] = readSizeY;

    // Assign the chunk/slab of data to the memory space i.e memspace
    status = H5Sselect_hyperslab (memspace, H5S_SELECT_SET, offset_out, NULL, 
                                    count_out, NULL);
    
    
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;

    // Define the size to read from the dataset
    count[0] = readSizeZ;
    count[1] = readSizeX;
    count[2] = readSizeY;
    int z = (unsigned long) dims_out[0];
    int x = (unsigned long) dims_out[1];
    int y = (unsigned long) dims_out[2];
    while((offset[1]+readSizeX<(x)) && (offset[2]+readSizeY<(y)) && (offset[0]+readSizeZ<(z)) ){

        // Define the offsets where to read from in the dataset

        printf("Reading from z:%lld x:%lld y:%lld\n",offset[0],offset[1],offset[2]);

        // Asign the slab in the dataset to the dataspace object
        status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, count, NULL);
        
        // Perform the actual read operation where the Data selected
        // in the dataspace is copied to the data in data out, where memsapce
        // is the metadata/attribute information of data_out

        // This line is equivalent to the reading of the chubk and copying it over to a smaller
        // array to subset it
        // The library handles the reading of the chunk and the subsetting. 
        // printf("Calling H5Dread\n");
        status = H5Dread (dataset, H5T_NATIVE_INT, memspace, dataspace,
                            H5P_DEFAULT, data_out);

        // printf("status: %d\n", status);
        offset[0]+= stepz;
        offset[1]+= stepx;
        offset[2]+= stepy;

    }
    // H5Dclose (dataset);
    // H5Sclose (dataspace);
    // H5Sclose (memspace);
    // H5Fclose (file);
}
