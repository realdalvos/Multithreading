/*==============================================================================

   Purpose:    2D gravitational N-body calculation (brute force)
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
         53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libFHBRS.h>

/*----------------------------------------------------------------------------*/
/* macros */

#define G 6.673e-11               // gravitational constant in m^3/(kg*s^2)
#define N_BODY 1000               // default number of bodies
#define BODY_MASS_FACTOR 1e26     // default body mass factor
#define BODY_VELOCITY_FACTOR 1e4  // default body velocity factor
#define BODY_DISTANCE_FACTOR 1e12 // default distance factor for positions
#define EPSILON 1e-5              // bodies must not come as close as this

#define SQR(x) ((x) * (x)) // square function as macro
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define CHECKSUM_REFERENCE 6956984643621UL

/*----------------------------------------------------------------------------*/
/* types */

// vector with 2 elements
typedef struct
{
  double x;
  double y;
} vector_t;

// one body
typedef struct
{
  vector_t position; // position vector (im m)
  vector_t velocity; // velocity vector
  vector_t force;    // force vector
  double mass;       // body mass (in kg)
} body_t;

/*----------------------------------------------------------------------------*/
/* variables */

static int display;         // show bodies on screen
static body_t *bodies;      // the bodies
static int n_body = N_BODY; // number of bodies
static double body_mass_factor = BODY_MASS_FACTOR;
static double body_velocity_factor = BODY_VELOCITY_FACTOR;
static double body_distance_factor = BODY_DISTANCE_FACTOR;

// actual time, start time, end time and time step (default 10 minute time step)
static double t, t_end = 1e100, dt = 10.0 * 60.0;

// (default) screen size
static int size_x = 800;
static int size_y = 800;

// remove old positions on screen?
static int remove_old_positions = 0;

// bounce on boundaries
static int bounce = 0;

static int use_solar_system = 0;
static body_t solar_system[] = {
    // position, velocity, force, mass (kg)
    {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, 1.989685296e30},          // sun
    {{5.790e10, 0.0}, {0.0, 4.787e4}, {0.0, 0.0}, 3.2868e23},      // merkur
    {{1.082e11, 0.0}, {0.0, 3.50e4}, {0.0, 0.0}, 4.87044e24},      // venus
    {{1.496e11, 0.0}, {0.0, 2.979e4}, {0.0, 0.0}, 5.976e24},       // earth
    {{2.279e11, 0.0}, {0.0, 2.413e4}, {0.0, 0.0}, 6.45408e23},     // mars
    {{7.783e11, 0.0}, {0.0, 1.31e4}, {0.0, 0.0}, 1.8997704e27},    // jupiter
    {{1.428e12, 0.0}, {0.0, 9.60e3}, {0.0, 0.0}, 5.68801656e26},   // saturn
    {{2.8966e12, 0.0}, {0.0, 6.80e3}, {0.0, 0.0}, 8.6837256e25},   // uranus
    {{4.4966e12, 0.0}, {0.0, 5.40e3}, {0.0, 0.0}, 1.0239876e26},   // neptun
    {{5.900e12, 0.0}, {0.0, 4.70e3}, {0.0, 0.0}, 1.1952e24},       // pluto
    {{-5.e12, -5.e12}, {1.1e4, 1.0e4}, {0.0, 0.0}, 1.989685296e30} // intruder :-)
};

#define SOLAR_LARGE (sizeof(solar_system) / sizeof(body_t))
#define SOLAR_SMALL 5

/*----------------------------------------------------------------------------*/
/* initialize bodies */

static void
init()
{
  int i;

  if (use_solar_system)
  {
    // large solar system
    n_body = SOLAR_LARGE - 1;
    bodies = solar_system;
    body_mass_factor = bodies[0].mass;
    body_velocity_factor = 5e4;
    body_distance_factor = 6e12;
    // 6 hour time steps
    dt = 6 * 60 * 60;

    if (use_solar_system == 2)
    {
      // small solar system
      n_body = SOLAR_SMALL;
      body_distance_factor = 3e11;
    }

    if (use_solar_system == 3)
    {
      // solar system with new star coming
      n_body = SOLAR_LARGE;
    }
  }

  else
  {
    // allocate memory for bodies
    bodies = malloc(n_body * sizeof(*bodies));
    if (bodies == NULL)
    {
      printf("no more memory\n");
      exit(1);
    }

    // initialize random number sequence
    rand_init(0);

    // initialize bodies
    for (i = 0; i < n_body; i++)
    {
      // random position vector in [-1,-1]x[+1,+1]
      bodies[i].position.x = (1.0 - 2.0 * rand_standard()) * body_distance_factor;
      bodies[i].position.y = (1.0 - 2.0 * rand_standard()) * body_distance_factor;

      // random velocity vector between -0.5 and 0.5 (in each direction)
      bodies[i].velocity.x = 2.0 * (0.5 - rand_standard()) * body_velocity_factor;
      bodies[i].velocity.y = 2.0 * (0.5 - rand_standard()) * body_velocity_factor;

      // force is zero
      bodies[i].force.x = bodies[i].force.y = 0.0;

      // random mass
      bodies[i].mass = rand_standard() * body_mass_factor;
    }
  }
}

/*----------------------------------------------------------------------------*/
/* version using symmetry of forces */

static void
calculate_forces()
{
  double distance, magnitude, factor, r;
  vector_t direction;
  int i, j;
#pragma omp parallel for private(j, r, magnitude, distance, direction, factor)
  for (i = 0; i < n_body - 1; i++)
  {
    for (j = i + 1; j < n_body; j++)
    {
#pragma omp critical(bodies)
      r = SQR(bodies[i].position.x - bodies[j].position.x) + SQR(bodies[i].position.y - bodies[j].position.y);
      // avoid numerical instabilities
      if (r < EPSILON)
      {
        // this is not how nature works :-)
        r += EPSILON;
      }
      distance = sqrt(r);
#pragma omp critical(bodies)
      magnitude = (G * bodies[i].mass * bodies[j].mass) / (distance * distance);

      factor = magnitude / distance;
#pragma omp critical(bodies)
      {
        direction.x = bodies[j].position.x - bodies[i].position.x;
        direction.y = bodies[j].position.y - bodies[i].position.y;

        // +force for body i
        bodies[i].force.x += factor * direction.x;
        bodies[i].force.y += factor * direction.y;

        // -force for body j
        bodies[j].force.x -= factor * direction.x;
        bodies[j].force.y -= factor * direction.y;
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

static void
move_bodies()
{
  vector_t delta_v, delta_p;
  int i;
#pragma omp parallel for private(delta_v, delta_p)
  for (i = 0; i < n_body; i++)
  {
    // calculate delta_v
    delta_v.x = bodies[i].force.x / bodies[i].mass * dt;
    delta_v.y = bodies[i].force.y / bodies[i].mass * dt;

    // calculate delta_p
    delta_p.x = (bodies[i].velocity.x + delta_v.x / 2.0) * dt;
    delta_p.y = (bodies[i].velocity.y + delta_v.y / 2.0) * dt;

    // update body velocity and position
    bodies[i].velocity.x += delta_v.x;
    bodies[i].velocity.y += delta_v.y;
    bodies[i].position.x += delta_p.x;
    bodies[i].position.y += delta_p.y;

    // reset forces
    bodies[i].force.x = bodies[i].force.y = 0.0;

    if (bounce)
    {
      // bounce on boundaries (i.e. it's more like billard)
      if ((bodies[i].position.x < -body_distance_factor) || (bodies[i].position.x > body_distance_factor))
        bodies[i].velocity.x = -bodies[i].velocity.x;
      if ((bodies[i].position.y < -body_distance_factor) || (bodies[i].position.y > body_distance_factor))
        bodies[i].velocity.y = -bodies[i].velocity.y;
    }
  }
}

/*----------------------------------------------------------------------------*/
// print bodies on screen

// map body positions to screen ccordinates
#define mapx(x) ((1.0 + (x / body_distance_factor)) * (size_x / 2))
#define mapy(y) ((1.0 + (y / body_distance_factor)) * (size_y / 2))

static void
show_bodies(int window)
{
  int i;
  static vector_t old_positions[N_BODY];
  int mx, my;

  if (display)
  {
    if (remove_old_positions)
    {
      // delete old bodies

      for (i = 0; i < n_body; i++)
      {
        // map to screen coordinates
        mx = mapx(bodies[i].position.x);
        my = mapy(bodies[i].position.y);
        graphic_setColor(window, GRAPHIC_WHITE);
        graphic_drawCircleFilled(window, old_positions[i].x, old_positions[i].y, MAX(1, (bodies[i].mass / body_mass_factor * 4.0)));

        old_positions[i].x = mx;
        old_positions[i].y = my;
      }
    }

    graphic_flush(window);

    for (i = 0; i < n_body; i++)
    {
      // draw bodies
      graphic_setColor(window, i % (GRAPHIC_MAX_COLOR - 1) + 1);
      graphic_drawCircleFilled(window, mapx(bodies[i].position.x), mapy(bodies[i].position.y), MAX(1, (bodies[i].mass / body_mass_factor * 5.0)));
    }

    graphic_flush(window);
  }
}

/*----------------------------------------------------------------------------*/

static void
usage(char *name)
{
  printf("usage: %s\n"
         "\t[-nodisplay]        don't display anything\n"
         "\t[-bodies n]         number of bodies (e.g. 20)\n"
         "\t[-size x y]         screen size (e.g. 800 600)\n"
         "\t[-velocityfactor v] velocity factor for bodies (e.g. 1e5)\n"
         "\t[-massfactor m]     mass factor for bodies (e.g. 1e31)\n"
         "\t[-distancefactor d] distance factor for bodies (e.g. 1e13)\n"
         "\t[-removeold]        remove old positions on screen\n"
         "\t[-timesteps n]      number of seconds for delta_t(e.g. 360 (10 minutes))\n"
         "\t[-t_end t]          end time in seconds(e.g. 3600 (1 hour))\n"
         "\t[-bounce]           bounce bodies on screen boundaries\n"
         "\t[solar_system]      use solar system\n"
         "\t[solar_system2]     use solar system with 5 inner planets only\n"
         "\t[solar_system3]     use solar system with an approaching new planet\n",
         name);
  exit(1);
}

/*----------------------------------------------------------------------------*/

static void
get_options(int argc, char **argv)
{
  int i;
  float f;

  display = 1;

  for (i = 1; i < argc; i++)
  {
    if (!strcmp("-nodisplay", argv[i]))
    {
      display = 0;
    }

    else if (!strcmp("-bodies", argv[i]))
    {
      // number of bodies
      if ((++i >= argc) || (sscanf(argv[i], "%d", &n_body) != 1))
        usage(argv[0]);
    }

    else if (!strcmp("-timesteps", argv[i]))
    {
      // timestep
      if ((++i >= argc) || (sscanf(argv[i], "%f", &f) != 1))
        usage(argv[0]);
      dt = f;
    }

    else if (!strcmp("-t_end", argv[i]))
    {
      // end time
      if ((++i >= argc) || (sscanf(argv[i], "%f", &f) != 1))
        usage(argv[0]);
      t_end = f;
    }

    else if (!strcmp("-size", argv[i]))
    {
      // size of screen
      if ((++i >= argc) || (sscanf(argv[i], "%d", &size_x) != 1))
        usage(argv[0]);
      if ((++i >= argc) || (sscanf(argv[i], "%d", &size_y) != 1))
        usage(argv[0]);
    }

    else if (!strcmp("-massfactor", argv[i]))
    {
      // number of bodies
      if ((++i >= argc) || (sscanf(argv[i], "%f", &f) != 1))
        usage(argv[0]);
      else
        body_mass_factor = f;
    }

    else if (!strcmp("-velocityfactor", argv[i]))
    {
      // number of bodies
      if ((++i >= argc) || (sscanf(argv[i], "%f", &f) != 1))
        usage(argv[0]);
      else
        body_velocity_factor = f;
    }

    else if (!strcmp("-distancefactor", argv[i]))
    {
      // number of bodies
      if ((++i >= argc) || (sscanf(argv[i], "%f", &f) != 1))
        usage(argv[0]);
      else
        body_distance_factor = f;
    }

    else if (!strcmp("-bounce", argv[i]))
      bounce = 1;

    else if (!strcmp("-removeold", argv[i]))
      remove_old_positions = 1;

    else if (!strcmp("-solar_system", argv[i]))
      use_solar_system = 1;

    else if (!strcmp("-solar_system2", argv[i]))
      use_solar_system = 2;

    else if (!strcmp("-solar_system3", argv[i]))
      use_solar_system = 3;

    else
      usage(argv[0]);
  }
}

/*----------------------------------------------------------------------------*/

static unsigned long
checksum()
{
  unsigned long checksum = 0;

  // initialize bodies
  for (int i = 0; i < n_body; i++)
  {
    // random position vector
    checksum += (unsigned long)round(bodies[i].position.x);
    checksum += (unsigned long)round(bodies[i].position.y);
  }

  return checksum;
}

/*----------------------------------------------------------------------------*/
/* main program */

int main(int argc, char **argv)
{
  double t0;
  int window = 0;

  get_options(argc, argv);
  init();
  if (display)
    window = graphic_start(size_x, size_y, "N-Body");

  /* time loop */
  t0 = gettime();
  for (t = 0; t < t_end; t += dt)
  {
    // draw bodies
    show_bodies(window);

    // computation
    calculate_forces();
    move_bodies();
  }

  // print out calculation speed every second
  t0 = gettime() - t0;
  printf("time nbody : %.6f\n", t0);
  unsigned long cs = checksum();
  if (abs(cs - CHECKSUM_REFERENCE) > 2)
    printf("error checksum wrong:\n"
           "\texpected=%lu\n"
           "\tseen    =%lu\n",
           CHECKSUM_REFERENCE, cs);
  else
    printf("checksum OK: %lu\n", cs);

  if (display)
    graphic_end(window);

  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
