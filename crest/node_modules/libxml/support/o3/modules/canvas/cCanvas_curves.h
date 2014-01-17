namespace o3{
    enum curve_recursion_limit_e { curve_recursion_limit = 32 };

	const double curve_distance_epsilon                  = 1e-30;
	const double curve_collinearity_epsilon              = 1e-30;
	const double curve_angle_tolerance_epsilon           = 0.01;

	__inline double calc_sq_distance(double x1, double y1, double x2, double y2)
	{
		o3_trace_scrfun("calc_sq_distance");
		double dx = x2-x1;
		double dy = y2-y1;
		return dx * dx + dy * dy;
	}

	struct QuadraticCurveGen
	{
		QuadraticCurveGen() : 
		  m_approximation_scale(1.0),
			  m_angle_tolerance(0.0),
			  m_count(0)
		  {
		  }

		  QuadraticCurveGen(double x1, double y1, double x2, double y2, double x3, double y3) :
		  m_approximation_scale(1.0),
			  m_angle_tolerance(0.0),
			  m_count(0)
		  { 
			  o3_trace_scrfun("QuadraticCurveGen"); 
			  init(x1, y1, x2, y2, x3, y3);
		  }


		  void init(double x1, double y1, double x2, double y2, double x3, double y3)
		  {
			  o3_trace_scrfun("init");
			  m_points.clear();
			  m_distance_tolerance_square = 0.5 / m_approximation_scale;
			  m_distance_tolerance_square *= m_distance_tolerance_square;
			  bezier(x1, y1, x2, y2, x3, y3);
			  m_count = 0;
		  };

		  unsigned vertex(double* x, double* y)
		  {
			  o3_trace_scrfun("vertex");
			  if(m_count >= m_points.size()) return agg::agg::path_cmd_stop;
			  V2<double> &p = m_points[m_count++];
			  *x = p.x;
			  *y = p.y;
			  return (m_count == 1) ? agg::agg::path_cmd_move_to : agg::agg::path_cmd_line_to;
		  }

		void recursive_bezier(double x1, double y1, double x2, double y2, double x3, double y3,unsigned level)
		{
			o3_trace_scrfun("recursive_bezier");
			if(level > curve_recursion_limit) 
			{
				return;
			}

			// Calculate all the mid-points of the line segments
			//----------------------
			double x12   = (x1 + x2) / 2;                
			double y12   = (y1 + y2) / 2;
			double x23   = (x2 + x3) / 2;
			double y23   = (y2 + y3) / 2;
			double x123  = (x12 + x23) / 2;
			double y123  = (y12 + y23) / 2;

			double dx = x3-x1;
			double dy = y3-y1;
			double d = fabs(((x2 - x3) * dy - (y2 - y3) * dx));
			double da;

			if(d > curve_collinearity_epsilon)
			{ 
				// Regular case
				if(d * d <= m_distance_tolerance_square * (dx*dx + dy*dy))
				{
					// If the curvature doesn't exceed the distance_tolerance value
					// we tend to finish subdivisions.
					if(m_angle_tolerance < curve_angle_tolerance_epsilon)
					{
						m_points.push(V2<double>(x123, y123));
						return;
					}

					// Angle & Cusp Condition
					//----------------------
					da = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
					if(da >= pi) da = 2*pi - da;

					if(da < m_angle_tolerance)
					{
						// Finally we can stop the recursion
						m_points.push(V2<double>(x123, y123));
						return;                 
					}
				}
			}
			else
			{
				// Collinear case
				da = dx*dx + dy*dy;
				if(da == 0)
				{
					d = calc_sq_distance(x1, y1, x2, y2);
				}
				else
				{
					d = ((x2 - x1)*dx + (y2 - y1)*dy) / da;
					if(d > 0 && d < 1)
					{
						// Simple collinear case, 1---2---3
						// We can leave just two endpoints
						return;
					}
					if(d <= 0) d = calc_sq_distance(x2, y2, x1, y1);
					else if(d >= 1) d = calc_sq_distance(x2, y2, x3, y3);
					else            d = calc_sq_distance(x2, y2, x1 + d*dx, y1 + d*dy);
				}
				if(d < m_distance_tolerance_square)
				{
					m_points.push(V2<double>(x2, y2));
					return;
				}
			}

			// Continue subdivision
			recursive_bezier(x1, y1, x12, y12, x123, y123, level + 1); 
			recursive_bezier(x123, y123, x23, y23, x3, y3, level + 1); 
		}

		void bezier(double x1, double y1, double x2, double y2, double x3, double y3)
		{
			//m_points.push(V2<double>(x1, y1)); // skip startpoint.. we use "curveTo" which implies the starting point is already there 
			o3_trace_scrfun("bezier");
			//m_points.push(V2<double>(x1, y1)); // skip startpoint.. we use "curveTo" which implies the starting point is already there 
			recursive_bezier(x1, y1, x2, y2, x3, y3, 0);
			m_points.push(V2<double>(x3, y3));
		}

		double               m_approximation_scale;
		double               m_distance_tolerance_square;
		double               m_angle_tolerance;
		unsigned             m_count;
		tVec<V2<double> > m_points;
	};

	struct BezierCurveGen
	{
		BezierCurveGen(double x1, double y1, 
			double x2, double y2, 
			double x3, double y3,
			double x4, double y4) :
		m_approximation_scale(1.0),
			m_angle_tolerance(0.0),
			m_cusp_limit(0.0),
			m_count(0)
		{ 
			o3_trace_scrfun("BezierCurveGen"); 
			init(x1, y1, x2, y2, x3, y3, x4, y4);
		}

		void init(double x1, double y1, 
			double x2, double y2, 
			double x3, double y3,
			double x4, double y4)
		{
			o3_trace_scrfun("init");
			m_points.clear();
			m_distance_tolerance_square = 0.5 / m_approximation_scale;
			m_distance_tolerance_square *= m_distance_tolerance_square;
			bezier(x1, y1, x2, y2, x3, y3, x4, y4);
			m_count = 0;
		}

		unsigned vertex(double* x, double* y)
		{
			o3_trace_scrfun("vertex");
			if(m_count >= m_points.size()) return agg::agg::path_cmd_stop;
			const V2<double> & p = m_points[m_count++];
			*x = p.x;
			*y = p.y;
			return (m_count == 1) ? agg::agg::path_cmd_move_to : agg::agg::path_cmd_line_to;
		}

		void recursive_bezier(double x1, double y1, 
			double x2, double y2, 
			double x3, double y3, 
			double x4, double y4,
			unsigned level)
		{
			o3_trace_scrfun("recursive_bezier");
			if(level > curve_recursion_limit) 
			{
				return;
			}

			// Calculate all the mid-points of the line segments
			//----------------------
			double x12   = (x1 + x2) / 2;
			double y12   = (y1 + y2) / 2;
			double x23   = (x2 + x3) / 2;
			double y23   = (y2 + y3) / 2;
			double x34   = (x3 + x4) / 2;
			double y34   = (y3 + y4) / 2;
			double x123  = (x12 + x23) / 2;
			double y123  = (y12 + y23) / 2;
			double x234  = (x23 + x34) / 2;
			double y234  = (y23 + y34) / 2;
			double x1234 = (x123 + x234) / 2;
			double y1234 = (y123 + y234) / 2;


			// Try to approximate the full cubic curve by a single straight line
			//------------------
			double dx = x4-x1;
			double dy = y4-y1;

			double d2 = fabs(((x2 - x4) * dy - (y2 - y4) * dx));
			double d3 = fabs(((x3 - x4) * dy - (y3 - y4) * dx));
			double da1, da2, k;

			switch((int(d2 > curve_collinearity_epsilon) << 1) +
				int(d3 > curve_collinearity_epsilon))
			{
			case 0:
				// All collinear OR p1==p4
				//----------------------
				k = dx*dx + dy*dy;
				if(k == 0)
				{
					d2 = calc_sq_distance(x1, y1, x2, y2);
					d3 = calc_sq_distance(x4, y4, x3, y3);
				}
				else
				{
					k   = 1 / k;
					da1 = x2 - x1;
					da2 = y2 - y1;
					d2  = k * (da1*dx + da2*dy);
					da1 = x3 - x1;
					da2 = y3 - y1;
					d3  = k * (da1*dx + da2*dy);
					if(d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
					{
						// Simple collinear case, 1---2---3---4
						// We can leave just two endpoints
						return;
					}
					if(d2 <= 0) d2 = calc_sq_distance(x2, y2, x1, y1);
					else if(d2 >= 1) d2 = calc_sq_distance(x2, y2, x4, y4);
					else             d2 = calc_sq_distance(x2, y2, x1 + d2*dx, y1 + d2*dy);

					if(d3 <= 0) d3 = calc_sq_distance(x3, y3, x1, y1);
					else if(d3 >= 1) d3 = calc_sq_distance(x3, y3, x4, y4);
					else             d3 = calc_sq_distance(x3, y3, x1 + d3*dx, y1 + d3*dy);
				}
				if(d2 > d3)
				{
					if(d2 < m_distance_tolerance_square)
					{
						m_points.push(V2<double>(x2, y2));
						return;
					}
				}
				else
				{
					if(d3 < m_distance_tolerance_square)
					{
						m_points.push(V2<double>(x3, y3));
						return;
					}
				}
				break;

			case 1:
				// p1,p2,p4 are collinear, p3 is significant
				//----------------------
				if(d3 * d3 <= m_distance_tolerance_square * (dx*dx + dy*dy))
				{
					if(m_angle_tolerance < curve_angle_tolerance_epsilon)
					{
						m_points.push(V2<double>(x23, y23));
						return;
					}

					// Angle Condition
					//----------------------
					da1 = fabs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2));
					if(da1 >= pi) da1 = 2*pi - da1;

					if(da1 < m_angle_tolerance)
					{
						m_points.push(V2<double>(x2, y2));
						m_points.push(V2<double>(x3, y3));
						return;
					}

					if(m_cusp_limit != 0.0)
					{
						if(da1 > m_cusp_limit)
						{
							m_points.push(V2<double>(x3, y3));
							return;
						}
					}
				}
				break;

			case 2:
				// p1,p3,p4 are collinear, p2 is significant
				//----------------------
				if(d2 * d2 <= m_distance_tolerance_square * (dx*dx + dy*dy))
				{
					if(m_angle_tolerance < curve_angle_tolerance_epsilon)
					{
						m_points.push(V2<double>(x23, y23));
						return;
					}

					// Angle Condition
					//----------------------
					da1 = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
					if(da1 >= pi) da1 = 2*pi - da1;

					if(da1 < m_angle_tolerance)
					{
						m_points.push(V2<double>(x2, y2));
						m_points.push(V2<double>(x3, y3));
						return;
					}

					if(m_cusp_limit != 0.0)
					{
						if(da1 > m_cusp_limit)
						{
							m_points.push(V2<double>(x2, y2));
							return;
						}
					}
				}
				break;

			case 3: 
				// Regular case
				//-----------------
				if((d2 + d3)*(d2 + d3) <= m_distance_tolerance_square * (dx*dx + dy*dy))
				{
					// If the curvature doesn't exceed the distance_tolerance value
					// we tend to finish subdivisions.
					//----------------------
					if(m_angle_tolerance < curve_angle_tolerance_epsilon)
					{
						m_points.push(V2<double>(x23, y23));
						return;
					}

					// Angle & Cusp Condition
					//----------------------
					k   = atan2(y3 - y2, x3 - x2);
					da1 = fabs(k - atan2(y2 - y1, x2 - x1));
					da2 = fabs(atan2(y4 - y3, x4 - x3) - k);
					if(da1 >= pi) da1 = 2*pi - da1;
					if(da2 >= pi) da2 = 2*pi - da2;

					if(da1 + da2 < m_angle_tolerance)
					{
						// Finally we can stop the recursion
						//----------------------
						m_points.push(V2<double>(x23, y23));
						return;
					}

					if(m_cusp_limit != 0.0)
					{
						if(da1 > m_cusp_limit)
						{
							m_points.push(V2<double>(x2, y2));
							return;
						}

						if(da2 > m_cusp_limit)
						{
							m_points.push(V2<double>(x3, y3));
							return;
						}
					}
				}
				break;
			}

			// Continue subdivision
			//----------------------
			recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1); 
			recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1); 
		}

		//------------------------------------------------------------------------
		void bezier(double x1, double y1, 
			double x2, double y2, 
			double x3, double y3, 
			double x4, double y4)
		{
			// m_points.push(V2<double>(x1, y1)); first point skipped in "curve to"
			o3_trace_scrfun("bezier");
			// m_points.push(V2<double>(x1, y1)); first point skipped in "curve to"
			recursive_bezier(x1, y1, x2, y2, x3, y3, x4, y4, 0);
			m_points.push(V2<double>(x4, y4));
		}

		double               m_approximation_scale;
		double               m_distance_tolerance_square;
		double               m_angle_tolerance;
		double               m_cusp_limit;
		unsigned             m_count;
		tVec<V2<double > > m_points;
	};

    struct ArcGen
	{
		ArcGen(double x,  double y, 
			double rx, double ry, 
			double a1, double a2, 
			bool ccw=true):

		m_x(x), m_y(y), m_rx(rx), m_ry(ry), m_scale(1.0)
		{
			o3_trace_scrfun("ArcGen");
			normalize(a1, a2, ccw);
			m_path_cmd = agg::agg::path_cmd_move_to; 
			m_angle = m_start;
			distancehad= 0;
		}


		unsigned vertex(double* x, double* y)
		{
			o3_trace_scrfun("vertex");
			if(agg::agg::is_stop(m_path_cmd)) return agg::agg::path_cmd_stop;
			double newdist = distancehad + distanceperstep;
			if(newdist  >= distance)
			{
				*x = m_x + cos(m_end) * m_rx;
				*y = m_y + sin(m_end) * m_ry;
				m_path_cmd = agg::agg::path_cmd_stop;
				return agg::agg::path_cmd_line_to;
			}
			distancehad = newdist;

			*x = m_x + cos(m_angle) * m_rx;
			*y = m_y + sin(m_angle) * m_ry;

			m_angle += m_da;

			unsigned pf = m_path_cmd;
			m_path_cmd = agg::agg::path_cmd_line_to;
			return pf;
		}

		void normalize(double a1, double a2, bool ccw)
		{
			o3_trace_scrfun("normalize");
			double ra = (fabs(m_rx) + fabs(m_ry)) / 2;
			
			m_da = acos(ra / (ra + 0.125 / m_scale)) * 2;
			
			distanceperstep = fabs(m_da);
			
			if(ccw)
			{
				distance = a2-a1;
			
			//	while(a2 < a1) a2 += pi * 2.0;
			}
			else
			{
				distance = a1-a2;
			//	while(a1 < a2) a1 += pi * 2.0;
				m_da = -m_da;
			}
			while (distance < 0 ) distance += pi * 2.0;
			m_ccw   = ccw;
			m_start = a1;
			m_end   = a2;
			m_initialized = true;
		}
		double distanceperstep;
		double distancehad;
		double distance;
		double   m_x;
		double   m_y;
		double   m_rx;
		double   m_ry;
		double   m_angle;
		double   m_start;
		double   m_end;
		double   m_scale;
		double   m_da;
		bool     m_ccw;
		bool     m_initialized;
		unsigned m_path_cmd;
	};
};