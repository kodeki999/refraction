PImage photo; //<>//

Sph pop;

void setup()
{
  size (736, 736);
  photo = loadImage("rcl.jpg");
  pop = new Sph (width/2, height/2.2, 10, 100);
  
}

void draw()
{

  loadPixels();
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int idx = x + y * width;
      if (!pop.IsIn(x, y)) {

        pixels[idx] = color(red(photo.pixels[idx]), green(photo.pixels[idx]), blue(photo.pixels[idx]));
      } else {

        PVector refra = pop.RFrah_rgb(x, y, width, height, 5);

        
        pixels[idx] = color(red(photo.pixels[int(refra.x)]), green(photo.pixels[int(refra.y)]), blue(photo.pixels[int(refra.z)]));
      }
 
    }
  }
  updatePixels();
  pop.change();
  

}
