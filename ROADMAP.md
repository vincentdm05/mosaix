
# Prototype

> Goal: Program that takes one reference image file as input, as well as a folder that contains all images, and outputs a mosaic image

- [ ] For all input images, crop to square and resize to tile size
- [ ] For all input images, compute mean colour, then store mapping colour -> image
- [ ] Split reference image into tiles, and compute mean colour for each tile
- [ ] Replace each tile with image based on closest colour between tile mean and mapping
- [ ] Output resulting image
- [ ] Allow choosing output image size
- [ ] Allow choosing size of tiles

# Usability

> Goal: Ease of use, debugging

- [ ] Make app with UI; 'Run' button as a minimum
- [ ] Input image and folder, as well as output file selectors
- [ ] Sliders and checkboxes for parameters
- [ ] View input image
- [ ] View result image
- [ ] Browse folder images
- [ ] Print stats

# More

> Goal: Add tasty features

- [ ] Image binning to avoid reuse
- [ ] Improved image crop based on basic facial recognition
- [ ] Colour shift
- [ ] Grayscale mode
- [ ] Random pick within selected range of closest colours
- [ ] Ability to save project (internal data, to avoid recomputing everything)
- [ ] Add an "infinity mode" that allows zooming in on individual tiles to reveal full-scale originals