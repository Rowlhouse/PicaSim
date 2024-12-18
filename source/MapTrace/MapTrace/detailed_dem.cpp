/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#define _CRT_SECURE_NO_WARNINGS

#include "detailed_dem.h"
#include "intersect.h"

#include <limits>
// hack for WIN32 - needs to come before some includes.
static const Detailed_dem::Val hack_val = std::numeric_limits<Detailed_dem::Val>::max();

#include <tiffio.h>

#include <cmath>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
using namespace std;

Detailed_dem::Detailed_dem(unsigned int size, 
                           float xmin, float ymin, float xmax, float ymax,
                           float resolution)
  :
  m_xmin(xmin),
  m_xmax(xmax),
  m_ymin(ymin),
  m_ymax(ymax),
  m_size(size),
  m_resolution(resolution)
{
  m_vals.resize(m_size*m_size);
  // work out zero offset - need to leave space for one bit at the top
  // for the sticky bit
  m_zero_offset =  hack_val / 4;
  
  // set everything to 0 height and non-sticky
  unsigned int i;
  unsigned int zero = m_zero_offset;
  for (i = 0 ; i < m_vals.size() ; ++i)
  {
    m_vals[i] = zero;
  }
}

void Detailed_dem::draw_ridge_line(float x0, float y0,
                                   float x1, float y1,
                                   float height, float width2,
                                   int id)
{
  // store the ridge line - they get used right at the end
  Ridge_line line = {x0, y0, x1, y1, height, width2, id, hypotf(x1-x0, y1-y0)};
  m_ridge_lines.push_back(line);
}


void Detailed_dem::draw_line(float x0, float y0,
                             float x1, float y1,
                             float height)
{
  // store the line
  Line line = {x0, y0, x1, y1, height, hypotf(x1-x0, y1-y0)};
  m_lines.push_back(line);
  
  int i0, j0, i1, j1;
  int i, j;
  
  float x_range = (m_xmax - m_xmin);
  float y_range = (m_ymax - m_ymin);
  
  i0 = (int) ( m_size * (x0 - m_xmin) / x_range);
  j0 = (int) ( m_size * (y0 - m_ymin) / y_range);
  i1 = (int) ( m_size * (x1 - m_xmin) / x_range);
  j1 = (int) ( m_size * (y1 - m_ymin) / y_range);
  
  float dx = x1 - x0;
  float dy = y1 - y0;
  
  // decide if we are going to move pixel by pixel horizontally or
  // vertically...
  bool move_hor = false;
  if (fabs(dx) > fabs(dy))
    move_hor = true;
  
  if (move_hor == true)
  {
    if (i1 < i0)
    {
      swap(i1, i0);
      swap(j1, j0);
      dx *= -1;
      dy *= -1;
    }
    else if (i1 == i0)
    {
      // just one point
      set_height(i0, j0, height);
      set_sticky(i0, j0);
      return;
    }
    // draw the line
    for (i = i0 ; i <= i1 ; ++i)
    {
      float dist_frac = ((float) i - i0) / (i1 - i0);
      // 0.49 is so that we get rounding to the nearest, rather than
      // floor.
      j = (int) (0.49 + j0 + dist_frac * (j1 - j0));
      set_height(i, j, height);
      set_sticky(i, j);
    }
  }
  else
  {
    if (j1 < j0)
    {
      swap(i1, i0);
      swap(j1, j0);
      dx *= -1;
      dy *= -1;
    }
    else if (j1 == j0)
    {
      // just one point
      set_height(i0, j0, height);
      set_sticky(i0, j0);
      return;
    }
    // draw the line
    for (j = j0 ; j <= j1 ; ++j)
    {
      float dist_frac = ((float) j - j0) / (j1 - j0);
      // 0.49 is so that we get rounding to the nearest, rather than
      // floor.
      i = (int) (0.49 + i0 + dist_frac * (i1 - i0));
      set_height(i, j, height);
      set_sticky(i, j);
    }
  }
}

void Detailed_dem::show() const
{
  cout << "size = " << m_size << endl;
  
  unsigned i, j;
  for ( j = 0 ; j < m_size ; ++j)
  {
    for ( i = 0 ; i < m_size ; ++i)
    {
      cout << setw(5) << get_height(i, j);
      if (get_sticky(i, j))
        cout << "*";
      else
        cout << " ";
    }
    cout << endl;
  }
}


void Detailed_dem::diffuse(float tol)
{
  do_initial_pass();
  cout << "Done initial pass\n";
  
  int i, j;
  
  // 1D diffusion is delat_z[i] = kapa * (z[i+1] - 2z[i] + z[i-1])
  // with kapa <= 0.5 for stability
  float kapa = 0.25;
  
  bool cont = true;
  int iterations = 0;
  printf("Iterations ");
  while (cont == true) 
  {
    printf("%d ", iterations++);
    cont = false;
    for (i = 0 ; i < (int) m_size ; ++i)
    {
      for (j = 0 ; j < (int) m_size ; ++j)
      {
        if (get_sticky(i, j) == false)
        {
          float h_p_0 = get_height(i+1, j);
          float h_m_0 = get_height(i-1, j);
          float h_0_p = get_height(i, j+1);
          float h_0_m = get_height(i, j-1);
          float h_0_0 = get_height(i, j);

          const float old_val = h_0_0;

#if 1
          float h_p_p = get_height(i+1, j+1);
          float h_m_m = get_height(i-1, j-1);
          float h_m_p = get_height(i-1, j+1);
          float h_p_m = get_height(i+1, j-1);

          float new_h = 0.125f * (h_p_0 + h_m_0 + h_0_p + h_0_m + h_p_p + h_m_m + h_p_m + h_m_p);
          set_height(i, j, new_h);
#else
          float delta_val = kapa * (h_m_0 + h_p_0 + h_0_m + h_0_p - 4 * h_0_0 );
          set_height(i, j, old_val + delta_val);
#endif
          // note that there will be precision errors...
          if (fabs(get_height(i, j) - old_val) > tol)
            cont = true;
        }
      }
    }
  }
  cout << "\nDone initial diffusion\n";
  
  //recalc_heights();
}

void Detailed_dem::recalc_heights()
{
  int i, j, count;
  
  cout << "Recalculating heights after diffusion\n";
  
  // The field will have ridges in it, but the slope is probably in
  // the correct direction. We use the slope to cast a ray from every
  // point (even sticky ones) in the direction of the slope to find
  // the nearest contours in each direction. Then we linearly
  // interpolate (we could fit a spline etc).

  vector<float> new_h(m_vals.size()); // we'll write into a new height field
  vector<float> Rx0(m_vals.size());
  vector<float> Ry0(m_vals.size());

  // At the end we're going to generate a new estimate for h based on
  // interpolation using the slope at the point. When the slope is
  // zero, this estimate is probably worse than the original (consider
  // a saddle point, where when the slope is zero it all goes
  // pear-shaped). So the final value is a weighted mean of the two
  // estimates.
  vector<float> slope_weight(m_vals.size());
  
  for (i = 0 ; i < (int) m_size ; ++i)
  {
    for (j = 0 ; j < (int) m_size ; ++j)
    {
      new_h[calc_index(i, j)] = get_height(i, j);
    }
  }
  
  // calculate the slope vector, D = (Dx, Dy), and the slope
  // direction R = del(D) / |del(D)|
  // we get our weights from this too
  for (i = 1 ; i < (int) m_size - 1 ; ++i)
  {
    for (j = 1 ; j < (int) m_size - 1 ; ++j)
    {
      float Dx = 0.5f * (get_height(i+1, j) - get_height(i-1, j));
      float Dy = 0.5f * (get_height(i, j+1) - get_height(i, j-1));
      float mag = hypotf(Dx, Dy);
      float weight = mag / (1.0f + mag);
      // factor 0.5 is arbitrary
      slope_weight[calc_index(i, j)] = powf(weight, 0.5f);
      
      if (mag > 0.0001f)
      {
        Rx0[calc_index(i, j)] = Dx/mag;
        Ry0[calc_index(i, j)] = Dy/mag;
      }
      else
      {
//        printf("Unable to estimate slope at (%d, %d)\n", i, j);
        // we will fill in by averaging in a moment
        Rx0[calc_index(i, j)] = 10.0;
        Ry0[calc_index(i, j)] = 0.0;
      }
    }
  }

  bool done = false;
  int i_off[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  int j_off[] = {1, 1, 1, 0, 0, -1, -1, -1};
  vector<float> Rx(Rx0);
  vector<float> Ry(Ry0);
  while (done == false)
  {
    done = true;
    for (i = 1 ; i < (int) m_size - 1 ; ++i)
    {
      for (j = 1 ; j < (int) m_size - 1 ; ++j)
      {
        if (Rx0[calc_index(i, j)] > 5.0)
        {
          int num = 0;
          float Rx_tot = 0.0f;
          float Ry_tot = 0.0f;
          for (int av_i = 0 ; av_i < 8 ; ++av_i)
          {
            int ii = i + i_off[av_i];
            int jj = j + j_off[av_i];
            if (Rx0[calc_index(ii, jj)] < 5.0)
            {
              ++num;
              Rx_tot += Rx0[calc_index(ii, jj)];
              Ry_tot += Ry0[calc_index(ii, jj)];
            }
          }
          if (num > 0)
          {
            float _Rx = Rx_tot/num;
            float _Ry = Ry_tot/num;
            float norm = hypotf(_Rx, _Ry);
            Rx[calc_index(i, j)] = _Rx/norm;
            Ry[calc_index(i, j)] = _Ry/norm;
          }
          else
          {
            // didn't have any neighbours.
            done = false;
          }
        }
      }
    }
    Rx0 = Rx;
    Ry0 = Ry;
  }

  // now walk through each point (except the edges...)
  for (i = 1 ; i < (int) m_size - 1 ; ++i)
  {
    for (j = 1 ; j < (int) m_size - 1 ; ++j)
    {
      float a0x = (float) i / m_size;
      float a0y = (float) j / m_size;
      
      float a1x = a0x + Rx[calc_index(i, j)];
      float a1y = a0y + Ry[calc_index(i, j)];
      
      float ta, tb;
      int pos_index = -1;
      float t_pos = 1e10;
      int neg_index = -1;
      float t_neg = -1e10;

      // walk through all the lines to find the closest
      for (count = m_lines.size(); count-- != 0 ; )
      {
        const float b0x = m_lines[count].x0;
        const float b0y = m_lines[count].y0;
        const float b1x = m_lines[count].x1;
        const float b1y = m_lines[count].y1;

        // do a quick check by checking the ends of the line segment
        float dist = min(fabs(b0x - a0x), fabs(b0y - a0y))
          - m_lines[count].len;
        if ( (dist > t_pos) && 
             (dist > (-t_neg) ) )
        {
          // check the other end
          dist = min(fabs(b1x - a0x), fabs(b1y - a0y)) 
            - m_lines[count].len;
          if ( (dist > t_pos) && 
               (dist > (-t_neg) ) )
          {
            continue; // skip
          }
        }
        
        
        if (true == intersect(a0x, a0y,
                              a1x, a1y,
                              b0x, b0y,
                              b1x, b1y,
                              ta,
                              tb) )
        {
          if ( (tb <= 1.0) && (tb >= 0.0) )
          {
            // we hit the line segment...
            if ( (ta > 0.0) && (ta < t_pos) )
            {
              // found the closest in the +ve dir
              t_pos = ta;
              pos_index = count;
            }
            else if ( (ta < 0.0) && (ta > t_neg) )
            {
              t_neg = ta;
              neg_index = count;
            }
            else if (ta == 0.0)
            {
              // right on top!
              t_pos = 0.0;
              pos_index = count;
              t_neg = 0.0;
              neg_index = count;
              // break out of the loop - we won't find anything closer
              break;
            }
          }
        } // intersection
      }  // loop over all lines.
//        printf("t- = %5.2f t+ %5.2f\n",
//               t_neg, t_pos);

      if ( (pos_index == -1) && (neg_index == -1) )
      {
        // didn't find anything - don't change
      }
      else if (pos_index == -1)
      {
        new_h[calc_index(i, j)] = m_lines[neg_index].h;
      }
      else if (neg_index == -1)
      {
        new_h[calc_index(i, j)] = m_lines[pos_index].h;
      }
      else
      {
        // found something in both directions
        if ( (t_pos - t_neg) == 0.0 )
        {
          new_h[calc_index(i, j)] = m_lines[pos_index].h;
          cout << "co-located\n";
        }
        else
        {
          new_h[calc_index(i, j)] = 
            m_lines[neg_index].h - t_neg * 
            (m_lines[pos_index].h - m_lines[neg_index].h) / 
            (t_pos - t_neg);
//            printf("h- = %5.2f, h+ = %5.2f, t- = %5.2f t+ %5.2f -> %5.2f\n",
//                   m_lines[neg_index].h, m_lines[pos_index].h,
//                   t_neg, t_pos, new_h[calc_index(i, j)]);
        }
      }
    } // loop over j
    cout << (m_size - i) << " ";
    
  } // over i
  cout << endl;
  
  // now write the new_h into the height field
  
  for (i = 0 ; i < (int) m_size ; ++i)
  {
    for (j = 0 ; j < (int) m_size ; ++j)
    {
      int index = calc_index(i, j);
      float new_height = 
        slope_weight[index] * new_h[index] + 
        (1.0f - slope_weight[index]) * get_height(i, j);
      set_height(i, j, new_height);
    }
  }
  cout << "Recalculated heights\n";

  add_ridges();
}

void Detailed_dem::add_ridges()
{
  cout << "Adding ridges\n";
  
  int old_id = -1;
  int ridge_i;
  
  // These form the "brush"
  vector<int> i_offsets;
  vector<int> j_offsets;
  vector<float> offsets;

  // ridge is an array of offsets associated with one ridge
  // (identified by id).
  vector<float> ridge(m_vals.size()); // we'll write into a new height field

  // ridge_total is the accumulation of the individual ridges
  vector<float> ridge_total(m_vals.size()); // we'll write into a new height field
  
  // underlying grid
  float delta_x = (m_xmax-m_xmin) / ((float) m_size - 1);
  float delta_y = (m_ymax-m_ymin) / ((float) m_size - 1);

  for (ridge_i = 0 ; ridge_i < (int) ridge.size() ; ++ridge_i)
  {
    ridge_total[ridge_i] = 0.0f;
    ridge[ridge_i] = 0.0f;
  }

  for (ridge_i = 0 ; ridge_i < (int) m_ridge_lines.size() ; ++ridge_i)
  {
//      printf("Ridge h = %f, w = %f, id = %d\n", 
//             m_ridge_lines[ridge_i].h,
//             m_ridge_lines[ridge_i].w,
//             m_ridge_lines[ridge_i].id);
    
    if (m_ridge_lines[ridge_i].id != old_id)
    {
      old_id = m_ridge_lines[ridge_i].id;
      printf("Found a new ridge id = %d\n", old_id);
      
      // a new id, set things up.
      i_offsets.clear();
      j_offsets.clear();
      offsets.clear();
      // copy the ridge into total before clearing it
      for (int ridge_i2 = 0 ; ridge_i2 < (int) ridge.size() ; ++ridge_i2)
      {
        ridge_total[ridge_i2] += ridge[ridge_i2];
        ridge[ridge_i2] = 0.0f;
      }
    
      int brush_width2 = 
        3 * (int) (m_ridge_lines[ridge_i].w/delta_x);
      
      for (int ii = -brush_width2 ; ii <= brush_width2 ; ++ii)
      {
        for (int jj = -brush_width2 ; jj <= brush_width2 ; ++jj)
        {
          float delx = ii * delta_x;
          float dely = jj * delta_y;
          i_offsets.push_back(ii);
          j_offsets.push_back(jj);
          float val = m_ridge_lines[ridge_i].h 
            * exp(-(delx*delx + dely*dely) 
                  / (m_ridge_lines[ridge_i].w * m_ridge_lines[ridge_i].w) );
          offsets.push_back(val);
//            printf("Brush (%d, %d) = %f for ridge h = %f, w = %f\n",
//                   ii, jj, val,
//                   m_ridge_lines[ridge_i].h,
//                   m_ridge_lines[ridge_i].w);
        }
      }
      // the brush is set up
    }
    // draw the line by updating each point according to the brush,
    // but only if the new value is > that the existing one (or < when
    // h is negative).

    // store the line in here, and process it afterwards
    vector<int> point_i;
    vector<int> point_j;

    int i0, j0, i1, j1;
    int i, j;
    
    float x_range = (m_xmax - m_xmin);
    float y_range = (m_ymax - m_ymin);
    
    float x0 = m_ridge_lines[ridge_i].x0;
    float y0 = m_ridge_lines[ridge_i].y0;
    float x1 = m_ridge_lines[ridge_i].x1;
    float y1 = m_ridge_lines[ridge_i].y1;

    i0 = (int) ( m_size * (x0 - m_xmin) / x_range);
    j0 = (int) ( m_size * (y0 - m_ymin) / y_range);
    i1 = (int) ( m_size * (x1 - m_xmin) / x_range);
    j1 = (int) ( m_size * (y1 - m_ymin) / y_range);
    
    float dx = x1 - x0;
    float dy = y1 - y0;
    
    // decide if we are going to move pixel by pixel horizontally or
    // vertically...
    bool move_hor = false;
    if (fabs(dx) > fabs(dy))
      move_hor = true;
    
    if (move_hor == true)
    {
      if (i1 < i0)
      {
        swap(i1, i0);
        swap(j1, j0);
        dx *= -1;
        dy *= -1;
      }
      else if (i1 == i0)
      {
        // just one point
        point_i.push_back(i0);
        point_j.push_back(j0);
        goto done;
      }
      // draw the line
      for (i = i0 ; i <= i1 ; ++i)
      {
        float dist_frac = ((float) i - i0) / (i1 - i0);
        // 0.49 is so that we get rounding to the nearest, rather than
        // floor.
        j = (int) (0.49 + j0 + dist_frac * (j1 - j0));
        point_i.push_back(i);
        point_j.push_back(j);
      }
    }
    else
    {
      if (j1 < j0)
      {
        swap(i1, i0);
        swap(j1, j0);
        dx *= -1;
        dy *= -1;
      }
      else if (j1 == j0)
      {
        // just one point
        point_i.push_back(i0);
        point_j.push_back(j0);
        goto done;
      }
      // draw the line
      for (j = j0 ; j <= j1 ; ++j)
      {
        float dist_frac = ((float) j - j0) / (j1 - j0);
        // 0.49 is so that we get rounding to the nearest, rather than
        // floor.
        i = (int) (0.49 + i0 + dist_frac * (i1 - i0));
        point_i.push_back(i);
        point_j.push_back(j);
      }
    }
    
    // got the points making up the line - use the brush on each one
  done:
    
    for (int p_index = 0 ; p_index < (int) point_i.size() ; ++p_index)
    {
      for (int ind = 0 ; ind < (int) i_offsets.size() ; ++ind)
      {
        int i = point_i[p_index] + i_offsets[ind];
        int j = point_j[p_index] + j_offsets[ind];
        if ( (i >= 0) && (i < (int) m_size) &&
             (j >= 0) && (j < (int) m_size) )
        {
          // i, j is valid
          float value = offsets[ind];
          if (fabs(value) > fabs(ridge[calc_index(i, j)]))
            ridge[calc_index(i, j)] = value;
        }
      } // looop over brush
    } // loop over points making up line
  } // loop over ridge lines
  
  // the last ridge won't have been added to the total - add it
  for (ridge_i = 0 ; ridge_i < (int) ridge.size() ; ++ridge_i)
  {
    ridge_total[ridge_i] += ridge[ridge_i];
  }

  // now add the ridge total to the output
  for (int i = 0 ; i < (int) m_size ; ++i)
  {
    for (int j = 0 ; j < (int) m_size ; ++j)
    {
//      printf("ridge_total(%d, %d) = %f\n", i, j, ridge_total[calc_index(i, j)]);
      set_height(i, j, get_height(i, j) + ridge_total[calc_index(i, j)]);
    }
  } 
  cout << "Done adding ridges\n";
}



void Detailed_dem::get_val_dist(
  int i0, int j0, float & val, int & dist, Dir dir)
{
  static int dis[] = {-1, 0, 1, 0};
  static int djs[] = {0, 1, 0, -1};
  
  int di = dis[(unsigned) dir];
  int dj = djs[(unsigned) dir];
  
  unsigned int i, j;
  dist = 0;
  val = 0;
  
  for ( i = i0, j = j0 ; 
        ( (i >= 0) && (i < m_size) && (j >= 0) && (j < m_size) ) ;
        i += di, j += dj)
  {
    ++dist;
    if (get_sticky(i, j) == true)
    {
      val = get_height(i, j);
      return;
    }
  }
  dist = -1;
}


void Detailed_dem::do_initial_pass()
{
  unsigned int i, j;
  // left to right
  
  for (j = 0 ; j < m_size ; ++j)
  {
    for (i = 0 ; i < m_size ; ++i)
    {
      if (get_sticky(i, j) == true)
        continue;
      
      // try to find a sticky value in 4 directions
      float val_right, val_left, val_up, val_down;
      int dist_right, dist_left, dist_up, dist_down;
      
      get_val_dist(i, j, val_right, dist_right, RIGHT);
      get_val_dist(i, j, val_left, dist_left, LEFT);
      get_val_dist(i, j, val_up, dist_up, UP);
      get_val_dist(i, j, val_down, dist_down, DOWN);
      
//        printf("(%3d, %3d) [%5f %3d] [%5f %3d] [%5f %3d] [%5f %3d] ->", 
//               i, j, 
//               val_left, dist_left,
//               val_right, dist_right,
//               val_up, dist_up,
//               val_down, dist_down
//               );
      
      float weight = 0;
      float val = 0;
      
      if (dist_right > 0)
      {
        val += val_right * 1.0f/dist_right;
        weight += 1.0f/dist_right;
      }
      if (dist_left > 0)
      {
        val += val_left * 1.0f/dist_left;
        weight += 1.0f/dist_left;
      }
      if (dist_up > 0)
      {
        val += val_up * 1.0f/dist_up;
        weight += 1.0f/dist_up;
      }
      if (dist_down > 0)
      {
        val += val_down * 1.0f/dist_down;
        weight += 1.0f/dist_down;
      }
      
      if (weight > 0)
      {
        val /= weight;
        set_height(i, j, val);
      }
    }
  }
}

void Detailed_dem::get_min_max_heights(float & hmin, float & hmax) const
{
  hmin = 999999999.0F;
  hmax = -999999999.0F;
  
  for (unsigned int i = 0 ; i < m_size ; ++i)
  {
    for (unsigned int j = 0 ; j < m_size ; ++j)
    {
      float h = get_height(i, j);
      if (h < hmin)
        hmin = h;
      if (h > hmax)
        hmax = h;
    }
  }
  // increase the range a bit... so we're not against the limits in
  // whatever we do.
  float h_range = hmax - hmin;
  hmin -= h_range * 0.05f;
  hmax += h_range * 0.05f;
}

TIFF* create_tiff_file(const string& file, int size)
{
  TIFF * tiff = TIFFOpen(file.c_str(), "w");
  if (tiff == 0)
  {
    cerr << "Unable to open " << file << endl;
    return 0;
  }
  
  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, size);
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, size);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 4);
//  TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 64);
  
  TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_LSB2MSB);
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  
  TIFFSetField(tiff, TIFFTAG_XRESOLUTION, 150.0);
  TIFFSetField(tiff, TIFFTAG_YRESOLUTION, 150.0);
  TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

  return tiff;
}

void Detailed_dem::do_output(const string & tiff_file,
                             const string & contour_file,
                             const string & orog_file,
                             int smooth_factor) const
{
  int out_size = m_size / smooth_factor;
  vector<uint32> buf(out_size);
  vector<uint32> bufC(m_size);
  int i, j;
  
  float hmin, hmax;
  get_min_max_heights(hmin, hmax);
  cout << hmin << " " << hmax << endl;
  
  // prepare for the .dat output
  int w = out_size;
  int h = out_size;
  
  TIFF* tiff = create_tiff_file(tiff_file, out_size);
  float * orog_z = new float[h*w];
  for (j = out_size-1 ; j >= 0 ; --j)
  {
    // set the values in the strip
    for (i = 0 ; i < out_size ; ++i)
    {
      float fval = 0;
      int ii, jj;
      for (ii = 0 ; ii < smooth_factor ; ++ii)
      {
        for (jj = 0 ; jj < smooth_factor ; ++jj)
        {
          fval += get_height( ii + (i * smooth_factor),
                              jj + (j * smooth_factor) );
        }
      }
      fval /= (smooth_factor * smooth_factor);
      
      // store the height
      orog_z[i + j * out_size] = fval;

      unsigned char cval = 
        (unsigned char) ((fval - hmin)/(hmax - hmin) * 255.0);
      
      buf[i] = 
        0xff000000 |
        cval << 16 |
        cval << 8 |
        cval ;
    }
    int rv = TIFFWriteScanline(tiff, &buf[0], out_size - (j + 1), 0);
    if (rv == -1)
    {
      cerr << "Error writing to TIFF file\n";
      TIFFClose(tiff);
      return;
    }
  }
  cout << "Written to " << tiff_file << endl;
  TIFFClose(tiff);

  // Write out a greyscale contour image too
  TIFF* contour = create_tiff_file(contour_file, m_size);
  for (j = m_size-1 ; j >= 0 ; --j)
  {
    for (int i = 0 ; i != m_size ; ++i)
    {
      bool contour = get_sticky(i, j);
      if (contour)
        bufC[i] = 0;
      else
        bufC[i] = 0xffffffff;
    }
    int rv = TIFFWriteScanline(contour, &bufC[0], m_size - (j + 1), 0);
    if (rv == -1)
    {
      cerr << "Error writing to TIFF file\n";
      TIFFClose(contour);
      return;
    }
  }
  
  cout << "Written to " << contour_file << endl;
  TIFFClose(contour);

  // ===================================================
  
  cout << "\n Now starting to write to " << orog_file << endl;
  
  FILE * o_file = fopen(orog_file.c_str(), "wb");
  if (!o_file)
  {
    cerr << "Unable to open " << o_file << endl;
    return;
  }
  
  fwrite(&orog_z[0], sizeof(orog_z[0]), w*h, o_file);
  
  delete [] orog_z;  
  
  fclose(o_file);
  printf("Written %s\n", orog_file.c_str());
  
}

