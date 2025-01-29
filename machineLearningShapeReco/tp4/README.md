# Modified version of the INRIA Holidays dataset

This dataset contains pictures of many locations, scenes or objects captured from several viewpoints.
It is used to assess the performance of content-based image retrieval systems, or near-duplicate detection systems.

This dataset was presented in the following paper:
> Herve Jegou, Matthijs Douze and Cordelia Schmid "Hamming Embedding and Weak geometry consistency for large scale image search" Proceedings of the 10th European conference on Computer vision, October, 2008


## Copyright
INRIA is the copyright holder of the original files.


## Contents
The current modified version contains a shuffled version of the dataset suitable for teaching.

It contains the following files.

For the student version:
- `jpg/*.jpg`: 1491 images;
- `thumbs/*.jpg`: image thumbnails for fast display;
- `siftgeo/*.siftgeo`: associated pre-computed SIFT descriptors for each image;
- `gt_student.json`: a partial ground truth to enable a partial evaluation of the retrieval;
- `queries_for_grading.json`: a list of queries for which you need to submit your results;
- `README.md`: this README file.


For the teacher version:
- all the files from the student version;
- `gt_full.json`: the complete ground truth file;
- `mapping_orig_to_shuffled.tsv`: the reference mapping between original filenames and modified file names.


## Ground truth format
The ground truth file is a JSON file. Here is an except:
```json
{
 "90314": [
  "90989",
  "91259"
 ],
 "90376": [
  "90674"
 ],
 ...
}
```
It contains a dictionary which associated to each query the list of relevant elements.
Every value is a string representing an image identifier, ie the part of the image or descriptor file without extension.

Example: `"90314"` is the identifier
of the image `jpgs/90314.jpg`
with descriptors available at `siftgeo/90314.siftgeo`
and its thumbnail is `thumbs/90314.jpg`.

Looking at the previous excerpt, we can see that the query `90314` has two relevant results: `90989` and `91259`.


## Descriptors format
Descriptors are stored in raw together with the region information provided by the software of Krystian Mikolajczyk.
There is no header (use the file length to find the number of descriptors).
A descriptor takes 168 bytes (floats and ints take 4 bytes, and are stored in little endian):
```
field      field type   description
x          float        horizontal position of the interest point
y          float        vertical position of the interest point
scale      float        scale of the interest region
angle      float        angle of the interest region
mi11       float        affine matrix component
mi12       float        affine matrix component
mi21       float        affine matrix component
mi22       float        affine matrix component
cornerness float        saliency of the interest point
desdim     int          dimension of the descriptors
component  byte*desdim  the descriptor vector (dd components)
```
