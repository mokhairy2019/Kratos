def calculate_pore_pressure_1D_consolidation(y_coord, H, T_v):
    """
    Calculates the analytical solution for water pressure in 1d consolidation on linear elastic soil [@@ref@@]

    :param y_coord: vertical coordinate
    :param H: sample height
    :param T_v:  dimensionless time factor
    :return: relative excess pore pressure
    """
    from math import fabs, cos, pi, exp

    convergence_criterion = 1e-10

    j = 1
    rel_p_old = 1
    rel_p = 0
    max_iterations = 1001
    min_iterations=20
    min_iterations_reached = False
    while fabs(rel_p_old - rel_p) > convergence_criterion and j < max_iterations or not min_iterations_reached:

        rel_p_old = rel_p
        rel_p = (-1) ** (j - 1) / (2 * j - 1) * cos((2 * j - 1) * pi / 2 * y_coord / H) * exp(
            -1 * (2 * j - 1) ** 2 * pi ** 2 / 4 * T_v) + rel_p_old
        j += 1

        if (j > min_iterations):
            min_iterations_reached = True

    rel_p = 4.0 / pi * rel_p
    return rel_p
    
def calculate_degree_of_1D_consolidation(H, T_v):
    """
    Calculates the analytical solution for degree of consolidatuion in 1d consolidation on linear elastic soil [@@ref@@]

    :param H: sample height
    :param T_v:  dimensionless time factor
    :return: degree of consolidation
    """
    from math import fabs, pi, exp

    convergence_criterion = 1e-10

    j = 1
    rel_d_old = 1
    rel_d = 0
    max_iterations = 1001
    min_iterations=20
    min_iterations_reached = False
    while fabs(rel_d_old - rel_d) > convergence_criterion and j < max_iterations or not min_iterations_reached:

        rel_d_old = rel_d
        rel_d = 1 / (2 * j - 1) ** 2 * exp(-1 * (2 * j - 1) ** 2 * pi ** 2 / 4 * T_v) + rel_d_old
        j += 1

        if (j > min_iterations):
            min_iterations_reached = True

    rel_d = 1.0 - 8.0 / (pi * pi) * rel_d
    return rel_d

def calculate_max_deflections_ring(force, r, young, m_inertia):
    """
    todo Extend description
    ref: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.912.904&rep=rep1&type=pdf
    :param force: Point load
    :param r: radius
    :param young: Young's modulus
    :param m_inertia: area moment of inertia
    :return: relative increase in horizontal and vertical diameter
    """
    from math import pi
    eps_h = (1/2 - 2/pi) * force * r ** 3 / (young*m_inertia)

    eps_v = (pi/4 - 2/pi) * force * r ** 3 / (young*m_inertia)
    return eps_h, eps_v

def calculate_bending_moments_ring(force, r, theta):
    """
    todo extend description
    ref http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.912.904&rep=rep1&type=pdf
    :param force: point load
    :param r: radius
    :param theta: angle
    :return: bending moment
    """
    from math import cos, pi
    moment = force * r * (1/pi - cos(theta)/2)
    return moment
