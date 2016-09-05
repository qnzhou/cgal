#ifndef CGAL_DATA_CLASSIFICATION_SEGMENTATION_ATTRIBUTE_ECHO_SCATTER_H
#define CGAL_DATA_CLASSIFICATION_SEGMENTATION_ATTRIBUTE_ECHO_SCATTER_H

#include <vector>


namespace CGAL {

  /*!
    \ingroup PkgDataClassification

    \brief Segmentation attribute based on echo scatter


    \tparam Kernel The geometric kernel used.

  */
template <typename Kernel, typename RandomAccessIterator, typename PointPMap, typename EchoPMap>
class Segmentation_attribute_echo_scatter : public Segmentation_attribute
{
  typedef Data_classification::Image<float> Image_float;
  typedef Data_classification::Planimetric_grid<Kernel, RandomAccessIterator, PointPMap> Grid;

  std::vector<double> echo_scatter;
  
public:
  /*!
    \brief Constructs the attribute.
  */
  Segmentation_attribute_echo_scatter (RandomAccessIterator begin,
                                       RandomAccessIterator end,
                                       EchoPMap echo_pmap,
                                       Grid& grid,
                                       const double grid_resolution,
                                       double radius_neighbors = 1.,
                                       double weight = 1.)
  {
    this->weight = weight;
    Image_float Scatter(grid.width(), grid.height());
    for (std::size_t j = 0; j < grid.height(); j++)
      for (std::size_t i = 0; i < grid.width(); i++)
        Scatter(i,j)=0;

    std::size_t square = (std::size_t)(0.5 * radius_neighbors / grid_resolution) + 1;

    for (std::size_t j = 0; j < grid.height(); j++){	
      for (std::size_t i = 0; i < grid.width(); i++){
						
        if(grid.mask(i,j)){

          std::size_t squareXmin = (i < square ? 0 : i - square);
          std::size_t squareXmax = std::min (grid.width()-1, i + square);
          std::size_t squareYmin = (j < square ? 0 : j - square);
          std::size_t squareYmax = std::min (grid.height()-1, j + square);
			
          int NB_echo_sup=0;
          int NB_echo_total=0;

          for(std::size_t k = squareXmin; k <= squareXmax; k++){
            for(std::size_t l = squareYmin; l <= squareYmax; l++){
									
              if(sqrt(pow((double)k-i,2)+pow((double)l-j,2))<=(double)0.5*radius_neighbors/grid_resolution){
										
                if(grid.indices(k,l).size()>0){
									
                  for(int t=0; t<(int)grid.indices(k,l).size();t++){
												
                    int ip = grid.indices(k,l)[t]; 
                    if(get(echo_pmap, begin[ip]) > 1)
                      NB_echo_sup++;
                  }
									
                  NB_echo_total=NB_echo_total+grid.indices(k,l).size();
									
                }
							
              }
						
            }
					
          }
					
          Scatter(i,j)=(float)NB_echo_sup/NB_echo_total;
				
        }
			
      }
		
    }
    for(std::size_t i = 0; i < (std::size_t)(end - begin); i++){
      int I= grid.x(i);
      int J= grid.y(i);
      echo_scatter.push_back((double)Scatter(I,J));
    }
    this->compute_mean_max (echo_scatter, this->mean, this->max);
  }
  
  virtual double value (std::size_t pt_index)
  {
    return echo_scatter[pt_index];
  }

  virtual std::string id() { return "echo_scatter"; }
};

}

#endif // CGAL_DATA_CLASSIFICATION_SEGMENTATION_ATTRIBUTE_ECHO_SCATTER_H
